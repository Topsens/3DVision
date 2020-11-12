#include <functional>
#include <vector>

class _cleanup
{
public:
    _cleanup(const std::function<void()>& func)
    {
        if (func)
        {
            this->func.push_back(func);
        }
    }
   ~_cleanup()
    {
        for (auto itr = this->func.rbegin(); itr != this->func.rend(); itr++)
        {
            (*itr)();
        }
    }

    void add(const std::function<void()>& func)
    {
        if (func)
        {
            this->func.push_back(func);
        }
    }

    void reset(const std::function<void()>& func)
    {
        this->func.clear();

        if (func)
        {
            this->func.push_back(func);
        }
    }

private:
    std::vector<std::function<void()>> func;
};
#define ONCLEANUP(X, F) _cleanup _cleanup_##X(F)
#define ADDCLEANUP(X, F) _cleanup_##X.add(F)
#define RESETCLEANUP(X, F) _cleanup_##X.reset(F)

// Usage:
//   when 'a' is out of code scope, clean up lambda function will be called to delete 'a'
//
//  {
//      int a = new int[10];
//      ONCLEANUP(a, [&]{ delete[] a; });
//  } // 'a' is deleted here