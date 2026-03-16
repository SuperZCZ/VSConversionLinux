/*!
 * FileName: ConsoleApplication1.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2025-2-20
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe:
 */

#include <stdio.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>


 // Macro definitions
#define PI 3.14159
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


#define offset_of(type, memb) \
    ((unsigned long)(&((type *)0)->memb))

#define container_of(ptr, type, member) ({			\
    const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
    (type *)( (char *)__mptr - offset_of(type,member) );})


struct dev
{
	char* name;
	int id;
	int number;
	unsigned char mode;
	unsigned int addr;
};
struct dev device;

// A simple class with constructors, methods, and a destructor
class SampleClass {
public:
	int publicInt;
	static int staticInt;

	SampleClass(int val) : publicInt(val) {}
	~SampleClass() {}

	void publicMethod() {
		std::cout << "Public method called, value: " << publicInt << std::endl;
	}


	//************************************
	// Method:    method1
	// FullName:  SampleClass::method1
	// Access:    public 
	// Returns:   void
	// Qualifier: 上方
	// Parameter: int a
	// Parameter: int b
	//************************************
	void method1(int a, int b);

	//************************************
	// Method:    staticMethod
	// FullName:  SampleClass::staticMethod
	// Access:    public static 
	// Returns:   void
	// Qualifier:
	// // 士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰
	// 士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰
	// 士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰士大夫石帆胜丰
	// end
	//************************************
	static void staticMethod() {
		std::cout << "Static method called, static value: " << staticInt << std::endl;
	}
};

struct MyStruct
{
	int a;
	int b;
};

int SampleClass::staticInt = 42;

//************************************
// Method:    method1
// FullName:  SampleClass::method1
// Access:    public 
// Returns:   void
// Qualifier: 定义
// Parameter: int a
// Parameter: int b
//************************************
void SampleClass::method1(int a, int b)
{
}

// A function template
template <typename T>
T add(T a, T b) {
	return a + b;
}

// Enumeration
enum DaysOfWeek { Sun, Mon, Tue, Wed, Thu, Fri, Sat };

// A simple structure
struct Point {
	int x;
	int y;
};

// Operator overloading
std::ostream& operator<<(std::ostream& os, const Point& p) {
	os << "Point(" << p.x << ", " << p.y << ")";
	return os;
}

void container_of_test()
{
	device.name = "Test";
	device.id = 3;
	device.number = 20;
	device.mode = 1;
	device.addr = 100;
	int s= sizeof(void*);
	int* address = &device.number;//定义一个指针指向number成员

	/*打印number成员的偏移量*/
	printf("offset_of =%d\n", offset_of(struct dev, number));

	/*根据number成员的地址求出结构体首地址*/
	printf("container_of =%d\n", container_of(address, struct dev, number));
}

// Main function
int main() {
	// Basic I/O and variables
	std::cout << "Hello, World!" << std::endl;
	int a = 10, b = 20;
	int sssbfdffff;

	// Using macros
	std::cout << "PI: " << PI << std::endl;
	std::cout << 5245 << "Squbbare of a: " << SQUARE(a) << std::endl;
	std::cout << "Max of a and b: " << MAX(a, b) << std::endl;

	// Conditionals
	if (a < b) {
		std::cout << "a is less than b" << std::endl;
	}

	// Loops
	for (int i = 0; i < 5; ++i) {
		std::cout << "i: " << i << std::endl;
	}


	MyStruct* st = new MyStruct;
	st->a = 1;
	SampleClass* ss = new SampleClass(1);
	ss->staticMethod();
	delete ss;

	// Using a class
	SampleClass obj(5);
	obj.publicMethod();
	obj.method1(1, 2);
	obj.method1(1, 2);
	obj.method1(1, 2);
	SampleClass::staticMethod();

	// Template function usage
	int sum = add<int>(a, b);
	std::cout << "Sum: " << sum << std::endl;

	// Using enumeration
	DaysOfWeek today = Wed;
	std::cout << "Today is day number: " << today << std::endl;

	// Using structure
	Point p = { 3, 4 };
	std::cout << p << std::endl;
	std::abort();

	// Vectors and algorithms
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	std::sort(vec.begin(), vec.end());
	for (int val : vec) {
		std::cout << "Vector element: " << val << std::endl;
	}

	// Lambda expressions
	auto lambda = [](int x, int y) { return x + y; };
	std::cout << "Lambda result: " << lambda(3, 4) << std::endl;
	printf("Press any key to continue...\n");
	return 0;
}