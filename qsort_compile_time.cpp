#include <functional>
#include <type_traits>
#include <utility>

// clang-format off

template<int Val>
using int_holder = std::integral_constant<int, Val>;

// Vector
template<int...>
struct Vector {};


// Push to the front of the vector
template<typename, typename>
struct PushFront {};

template<typename ValueHolder, int... Elems>
struct PushFront<ValueHolder, Vector<Elems...>> {
  using type = Vector<ValueHolder::value, Elems...>;
};


// GetElement of the vector by Index
template<int Index, typename Vec>
struct GetElement {};

template<int... Elems>
struct GetElement< -1, Vector<Elems...>> {
  static_assert(sizeof...(Elems) == -1, "GetElement index out of bounds");
};

template<int Elem, int... Elems>
struct GetElement<0, Vector<Elem, Elems...>> {
  static constexpr int value = Elem;
};

template<int Index, int Elem, int... Elems>
struct GetElement<Index, Vector<Elem, Elems...>> {
  static constexpr int value = GetElement<Index - 1, Vector<Elems...>>::value;
};

// Two vectors concatenation
template<typename, typename>
struct ConcatinateVectors {};

template<int... Elems1, int... Elems2>
struct ConcatinateVectors<Vector<Elems1...>, Vector<Elems2...>> {
  using type = Vector<Elems1..., Elems2...>;
};


// Remove Target from the Vector
template<typename Vector, typename Comparator, typename Target, typename ResultVec>
struct RemoveIf {};

template<typename Comparator, typename Target, typename ResultVec>
struct RemoveIf<Vector<>, Target, Comparator, ResultVec> {
  using type = ResultVec;
};

template<typename Comparator, typename Target, typename ResultVec, int Elem, int... Elems>
struct RemoveIf<Vector<Elem, Elems...>, Target, Comparator, ResultVec> {
  using type = typename std::conditional_t<
     /* condition */ Comparator()(Elem, Target::value),
     /* true branch */ RemoveIf<Vector<Elems...>, Target, Comparator, typename PushFront<int_holder<Elem>, ResultVec>::type>,
     /* false branch */ RemoveIf<Vector<Elems...>, Target, Comparator, ResultVec>
  >::type;
};


// Helper class for sorting
template<typename>
struct QsortHelper {};

template<>
struct QsortHelper<Vector<>> {
  using type = Vector<>;
};

template<int... Elems>
struct QsortHelper<Vector<Elems...>> {
private:
  static constexpr int Size = sizeof...(Elems);

  static constexpr int Pivot = GetElement<Size / 2, Vector<Elems...>>::value;
  using LowPart = typename RemoveIf<Vector<Elems...>, int_holder<Pivot>, std::less<int>, Vector<>>::type;
  using MidPart = typename RemoveIf<Vector<Elems...>, int_holder<Pivot>, std::equal_to<int>, Vector<>>::type;
  using HighPart = typename RemoveIf<Vector<Elems...>, int_holder<Pivot>, std::greater<int>, Vector<>>::type;

public:
  using type = typename ConcatinateVectors<
      typename ConcatinateVectors<typename QsortHelper<LowPart>::type, MidPart>::type,
      typename QsortHelper<HighPart>::type>
          ::type;
};

// Class for sorting
template<typename Vec>
struct Qsort {};

template<int... Elems>
struct Qsort<Vector<Elems...>> {
  using type = typename QsortHelper<Vector<Elems...>>::type;
};

// clang-format on

int main() {
  // Test #1
  {
    using Input = Vector<>;
    using Sorted = Qsort<Input>::type;
    static_assert(std::is_same<Sorted, Vector<>>::value, "Test 1 failed");
  }

  // Test #2
  {
    using Input = Vector<2>;
    using Sorted = Qsort<Input>::type;
    static_assert(std::is_same<Sorted, Vector<2>>::value, "Test 2 failed");
  }

  // Test #3
  {
    using Input = Vector<5, 1, 67, 5, 2, 3, 1, 1, 1, 322>;
    using Sorted = Qsort<Input>::type;
    static_assert(
        std::is_same<Sorted, Vector<1, 1, 1, 1, 2, 3, 5, 5, 67, 322>>::value,
        "Test 23 failed");
  }

  // Test #4
  {
    using Input = Vector<2, -2>;
    using Sorted = Qsort<Input>::type;
    static_assert(std::is_same<Sorted, Vector<-2, 2>>::value, "Test 4 failed");
  }

  // Test #5
  {
    using Input = Vector<20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6,
                         5, 4, 3, 2, 1>;
    using Sorted = Qsort<Input>::type;
    static_assert(
        std::is_same<Sorted, Vector<1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                                      14, 15, 16, 17, 18, 19, 20>>::value,
        "Test 5 failed");
  }

  // Test #6
  {
    using Input = Vector<20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6,
                         5, 4, 3, 2, 1, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
                         10, 9, 8, 7, 6, 5, 4, 3, 2, 1>;
    using Sorted = Qsort<Input>::type;
    static_assert(
        std::is_same<Sorted,
                       Vector<1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9,
                              9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
                              16, 16, 17, 17, 18, 18, 19, 19, 20, 20>>::value,
        "Test 23 failed");
  }

  return 0;
}
