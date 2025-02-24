
//#include "header.hpp"

void test1()
{
  bool ret = Solution().isPalindrome(121);
  if (ret)
  {
    std::cout << "测试121通过" << std::endl;
  }
  else
  {
    std::cout << "测试121没通过" << std::endl;
  }
}
void test2()
{
  bool ret = Solution().isPalindrome(-10);
  if (ret)
  {
    std::cout << "测试-10通过" << std::endl;
  }
  else
  {
    std::cout << "测试-10没通过" << std::endl;
  }
}
int main()
{

  test1();

  test2();

  return 0;
}