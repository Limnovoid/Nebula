#ifndef NEBULA_TEST_HANDLER_2_H
#define NEBULA_TEST_HANDLER_2_H

#include "NebulaTypes.h"
#include "Macros.h"
#include "ConstString.h"
#include "UiMenu.h"
#include "Maths.h"

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class UiMenu;
class ITest;
template<typename CRTP> class TTest;

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
concept CTest = requires
{
	CDerivesFrom<T, TTest<T>>;
	CConstString<decltype(T::NAME)>;
	0 != T::NAME.Size();
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class Suite
{
public:
	Suite(StringView suiteName) : m_pMenu(MakeShared<UiMenu>(suiteName)) {}

	template<CTest TTest>
	bool Register()
	{
		const auto &[testMapIter, isNewElement] = m_testMap.emplace(StringView(TTest::NAME.Get()), MakeUnique<TTest>());

		const StringView testName = testMapIter->first;
		ITest const& testInterface = *testMapIter->second;

		if (isNewElement)
			m_pMenu->AddOption(MakeShared<TestOption>(testInterface, testName));

		return isNewElement;
	}

	SharedPtr<UiMenu> GetMenu() { return m_pMenu; }

private:
	class TestOption : public IOption
	{
	public:
		TestOption(ITest const& testInterface, const StringView testName) : m_testInterface(testInterface), m_testName(testName) {}

		virtual StringView GetPrompt() const override { return m_testName; }

		virtual void Execute(UiIo const& ui) override;

	private:
		ITest const&		m_testInterface;
		const StringView	m_testName;
	};

	using Children = std::vector<UniquePtr<Suite>>;
	using TestMap = std::unordered_map<StringView, UniquePtr<ITest>>;

	Children			m_childSuites;

	TestMap				m_testMap;
	SharedPtr<UiMenu>	m_pMenu;
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class TestRegister
{
public:
	using SuiteMap = std::unordered_map<StringView, SharedPtr<Suite>>;

	template<CTest TTest>
	static bool Register();

	static SuiteMap & GetSuiteMap();
	static SharedPtr<UiMenu> GetMenu();

	inline static ConstString ROOT_SUITE_NAME = "ROOT";

private:
	TestRegister() = default;

	inline static ConstString ROOT_SUITE_MENU_NAME = "Testing";
};

// --------------------------------------------------------------------------------------------------------------------------------

template<CTest TTest>
inline bool TestRegister::Register()
{
	SuiteMap & suiteMap = GetSuiteMap();
	const StringView suiteName(TTest::SUITE.Get());

	SuiteMap::iterator suiteIter = suiteMap.find(suiteName);

	if (suiteMap.end() == suiteIter)
	{
		suiteIter = suiteMap.emplace(suiteName, MakeShared<Suite>(suiteName)).first;

		GetMenu()->AddOption(suiteIter->second->GetMenu());
	}

	return suiteIter->second->Register<TTest>();
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class IOperator
{
public:
	IOperator(T const& rhsOperand);

	virtual bool operator()(T const& lhs) const = 0;

	virtual const StringView ToString() const = 0;

	T const&	m_rhs;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline IOperator<T>::IOperator(T const& rhsOperand) :
	m_rhs(rhsOperand)
{
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class Equal : public IOperator<T>
{
public:
	Equal(T const& rhsOperand) : IOperator<T>(rhsOperand) {}

	virtual bool operator()(T const& lhs) const override { return (lhs == this->m_rhs); }

	virtual const StringView ToString() const override { return "=="; }
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class NotEqual : public IOperator<T>
{
public:
	NotEqual(T const& rhsOperand) : IOperator<T>(rhsOperand) {}

	virtual bool operator()(T const& lhs) const override { return (lhs != this->m_rhs); }

	virtual const StringView ToString() const override { return "!="; }
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class TestLogger
{
public:
	using LogType = uint8_t;

	enum : LogType
	{
		LOG_TYPE_INFO,
		LOG_TYPE_WARN,
		LOG_TYPE_FAIL,
	};

	static std::ostream & Get(LogType logType)
	{
		switch (logType)
		{
		case LOG_TYPE_INFO:
		case LOG_TYPE_WARN:
			return std::cout;

		case LOG_TYPE_FAIL:
			return std::cerr;

		default:
			break;
		}

		throw std::exception(); // We shouldn't be here!
	}

	static std::ostream & GetPreambled(LogType logType)
	{
		std::ostream & ostream = Get(logType);

		switch (logType)
		{
		case LOG_TYPE_INFO:
			ostream << "Test info: ";
			break;

		case LOG_TYPE_WARN:
			ostream << "Test WARN: ";
			break;

		case LOG_TYPE_FAIL:
			ostream << "Test FAIL: ";
			break;

		default: break;
		}

		return ostream;
	}

	static void LogError(const StringView message)
	{
		GetPreambled(LOG_TYPE_FAIL) << message << std::endl;
	}
};

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

class ITest
{
public:
	using SequenceType = uint8_t;
	enum : SequenceType
	{
		SEQUENCE_TYPE_INCREMENTAL,
		SEQUENCE_TYPE_DECREMENTAL,
		SEQUENCE_TYPE_RANDOMIZED,
	};

	class Sequence
	{
	public:
		using Index = int64_t;
		using Vector = std::vector<Index>;

		class ConstIterator
		{
			friend class Sequence;

		public:
			ConstIterator(ConstIterator const& rhs) : m_internalIndex(rhs.m_internalIndex), m_indices(rhs.m_indices) {}

			Index GetInternalIndex() const { return m_internalIndex; }
			Vector const& GetIndices() const { return m_indices; }

			bool operator==(ConstIterator const& rhs) const { return (m_indices.data() == rhs.m_indices.data()) && (m_internalIndex == rhs.m_internalIndex); }
			Index const& operator*() const { return m_indices[m_internalIndex]; }
			ConstIterator & operator++() { ++m_internalIndex; return *this; }
			ConstIterator operator++(int) { ConstIterator temp(*this); ++m_internalIndex; return temp; }

		private:
			ConstIterator(const Index index, Vector const& indices) : m_internalIndex(index), m_indices(indices) {}

			Index			m_internalIndex;
			Vector const&	m_indices;
		};

		Sequence(const Index min, const Index max, const size_t stepSize, const SequenceType type = SEQUENCE_TYPE_INCREMENTAL);

		/// <returns> The number of indices in the sequence. </returns>
		size_t Length() const;

		/// <returns> The vector of sequenced indices. </returns>
		Vector const& GetIndices() const;
		const Index GetMaxIndex() const;
		const Index GetMinIndex() const;

		ConstIterator cbegin() const;	///< STL compatibility.
		ConstIterator cend() const;		///< STL compatibility.

	private:
		Vector			m_indices;		/// The sequence of indices.
		const Index		m_maxIndex;		/// The maximum index.
		const Index		m_minIndex;		/// The minimum index.
	};

	using SequenceOperation = std::function<void(const Sequence::Index)>;

	virtual ~ITest() = default;

	virtual void Run() const = 0;

protected:
	void Assert(const StringView title, const bool expression) const;

	template<typename T>
	void AssertOperator(const StringView title, T const& lhs, IOperator<T> const& op) const;

	void AssertSequence(const StringView title, Sequence const& sequence, SequenceOperation const& operation) const;

private:
	String GetErrorMessage(const StringView title) const;

	String GetErrorMessage(Sequence const& sequence, Sequence::ConstIterator citerator, const StringView title) const;

	template<typename T>
	String GetErrorMessage(const StringView title, T const& lhs, IOperator<T> const& op) const;

	template<typename T>
	String GetErrorMessage(Sequence const& sequence, Sequence::ConstIterator citerator, const StringView title, T const& lhs, IOperator<T> const& op) const;

	String GetErrorMessageIteratorPreamble(Sequence const& sequence, Sequence::ConstIterator citerator) const;

	using SequenceStack = std::stack<std::reference_wrapper<const Sequence>>;
	using SequenceIteratorStack = std::stack<Sequence::ConstIterator>;

	mutable SequenceStack				m_sequenceStack;			/// The sequence stack.
	mutable SequenceIteratorStack		m_sequenceIteratorStack;	/// The sequence iterator stack.
};

// --------------------------------------------------------------------------------------------------------------------------------

inline void ITest::Assert(const StringView title, const bool expression) const
{
	if (!expression)
	{
		assert(m_sequenceStack.empty() && m_sequenceIteratorStack.empty());

		if (m_sequenceStack.empty() && m_sequenceIteratorStack.empty())
			TestLogger::LogError(GetErrorMessage(title));
		else
			TestLogger::LogError(GetErrorMessage(m_sequenceStack.top(), m_sequenceIteratorStack.top(), title));
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline void ITest::AssertOperator(const StringView title, T const& lhs, IOperator<T> const& op) const
{
	const bool result = op(lhs);

	if (!result)
	{
		if (m_sequenceStack.empty())
			TestLogger::LogError(GetErrorMessage(title, lhs, op));
		else
			TestLogger::LogError(GetErrorMessage(m_sequenceStack.top(), m_sequenceIteratorStack.top(), title, lhs, op));
	}
}

// --------------------------------------------------------------------------------------------------------------------------------

inline void ITest::AssertSequence(const StringView title, Sequence const& sequence, SequenceOperation const& operation) const
{
	TestLogger::Get(TestLogger::LOG_TYPE_INFO) << "Beginning sequence '" << title << "' ..." << std::endl;

	m_sequenceStack.push(sequence);

	for (Sequence::ConstIterator citerator = sequence.cbegin(); sequence.cend() != citerator; ++citerator)
	{
		m_sequenceIteratorStack.push(citerator);

		operation(*citerator);

		m_sequenceIteratorStack.pop();
	}

	m_sequenceStack.pop();

	TestLogger::Get(TestLogger::LOG_TYPE_INFO) << "Sequence '" << title << "' complete." << std::endl;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String ITest::GetErrorMessage(const StringView title) const
{
	return Fmt::Format("'{}'", title);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String ITest::GetErrorMessage(Sequence const& sequence, Sequence::ConstIterator citerator, const StringView title) const
{
	return Fmt::Format("{} : '{}'", GetErrorMessageIteratorPreamble(sequence, citerator).c_str(), title);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline String ITest::GetErrorMessage(const StringView title, T const& lhs, IOperator<T> const& op) const
{
	return Fmt::Format("'{}' [ {} {} {} ]", title, lhs, op.ToString(), op.m_rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline String ITest::GetErrorMessage(Sequence const& sequence, Sequence::ConstIterator citerator, const StringView title, T const& lhs, IOperator<T> const& op) const
{
	return Fmt::Format("{} : '{}' [ {} {} {} ]",
		GetErrorMessageIteratorPreamble(sequence, citerator).c_str(), title, lhs, op.ToString(), op.m_rhs);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline String ITest::GetErrorMessageIteratorPreamble(Sequence const& sequence, Sequence::ConstIterator citerator) const
{
	const size_t indexPadding = std::max(Maths::NumDigits(sequence.GetMinIndex()), Maths::NumDigits(sequence.GetMaxIndex()));
	const size_t internalIndexPadding = Maths::NumDigits(citerator.GetIndices().size());

	return Fmt::Format("Index {:<{}} ({:<{}})", *citerator, indexPadding, citerator.GetInternalIndex(), internalIndexPadding);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

inline size_t ITest::Sequence::Length() const
{
	return m_indices.size();
}

// --------------------------------------------------------------------------------------------------------------------------------

inline std::vector<ITest::Sequence::Index> const& ITest::Sequence::GetIndices() const
{
	return m_indices;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const ITest::Sequence::Index ITest::Sequence::GetMaxIndex() const
{
	return m_maxIndex;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline const ITest::Sequence::Index ITest::Sequence::GetMinIndex() const
{
	return m_minIndex;
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ITest::Sequence::ConstIterator ITest::Sequence::cbegin() const
{
	return ConstIterator(0, m_indices);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline ITest::Sequence::ConstIterator ITest::Sequence::cend() const
{
	return ConstIterator(m_indices.size(), m_indices);
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

template<typename CRTP>
class TTest : public ITest
{
public:
	inline static ConstString NAME = "";
	inline static ConstString SUITE = TestRegister::ROOT_SUITE_NAME;

	static const bool IS_REGISTERED;

	virtual ~TTest() = 0;

private:
	// Force the static member variable to be initialized in order to register the derived test class:
	template<typename T, T> struct TForceInitialize {};
	typedef TForceInitialize<bool const&, IS_REGISTERED> ForceInitialize;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename CRTP>
inline TTest<CRTP>::~TTest()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

template<typename CRTP>
const bool TTest<CRTP>::IS_REGISTERED = TestRegister::Register<CRTP>();

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TEST_HANDLER_2_H
