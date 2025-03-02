
namespace npropery {
struct reflect {};

} // namespace npropery

struct Reflect {

    [[npropery::reflect]] int value;

    [:a:] int a;
    // will be a lambda, wtf!!
    // [abc] Reflect()
    // {
    // }
};

int main()
{
    Reflect r;
    r.value = 1;
}