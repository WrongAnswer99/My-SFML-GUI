#include "../include/engine/data/VarianTmap.hpp"
#include <cassert>
#include <iostream>
#include <string>

struct TestBase {
    virtual ~TestBase() = default;
    virtual std::string name() const = 0;
};
struct TestA : TestBase {
    int value = 0;
    TestA() = default;
    explicit TestA(int v) : value(v) {}
    std::string name() const override { return "A"; }
};
struct TestB : TestBase {
    std::string text;
    TestB() = default;
    explicit TestB(std::string t) : text(std::move(t)) {}
    std::string name() const override { return "B"; }
};

int main() {
    // ===== 1. 插入 =====
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("item1", TestA{42});
        assert(a != nullptr);
        assert(a->value == 42);
        assert(map.size() == 1);
        std::cout << "Test 1.1 passed: push_back with key." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back(TestA{10});
        auto* b = map.push_back(TestB{});
        assert(a != nullptr);
        assert(b != nullptr);
        assert(map.size() == 2);
        std::cout << "Test 1.2 passed: push_back without key." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("item1", TestA{});
        bool caught = false;
        try {
            map.push_back<TestB>("item1", TestB{}); // 同名 key 应报错
        } catch (const std::runtime_error&) {
            caught = true;
        }
        assert(caught);
        std::cout << "Test 1.3 passed: duplicate key throws." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_front<TestA>("first", TestA{1});
        auto* b = map.push_back<TestA>("second", TestA{2});
        assert(map.size() == 2);
        assert(map.order().front() == a);
        assert(map.order().back() == b);
        std::cout << "Test 1.4 passed: push_front/push_back order." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("a", TestA{1});
        map.insert<TestB>(a, "b", TestB{"hello"});
        assert(map.size() == 2);
        // b 应在 a 之前
        assert(map.order().front() != a); // 新插入的在前面
        std::cout << "Test 1.5 passed: insert before." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.emplace<TestA>(map.end(), "emplaced", 99);
        assert(a != nullptr);
        assert(a->value == 99);
        assert(map.size() == 1);
        std::cout << "Test 1.6 passed: emplace." << std::endl;
    }

    // ===== 2. 查找 =====
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("myKey", TestA{77});
        // 无泛型 find
        TestBase* found = map.find("myKey");
        assert(found != nullptr);
        assert(found->name() == "A");
        // 泛型 find
        TestA* foundA = map.find<TestA>("myKey");
        assert(foundA != nullptr);
        assert(foundA->value == 77);
        // 不存在的 key
        assert(map.find("noKey") == nullptr);
        // 类型不匹配
        assert(map.find<TestB>("myKey") == nullptr);
        std::cout << "Test 2.1 passed: find by key." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("a", TestA{1});
        // 通过指针查找
        assert(map.find(a) == a);
        assert(map.find<TestA>(a) == a);
        assert(map.find<TestB>(a) == nullptr);
        std::cout << "Test 2.2 passed: find by pointer." << std::endl;
    }

    // ===== 3. find_order =====
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("a", TestA{1});
        auto* b = map.push_back<TestA>("b", TestA{2});
        auto it = map.find_order("a");
        assert(it != map.order().end());
        assert(*it == a);
        it = map.find_order("b");
        assert(it != map.order().end());
        assert(*it == b);
        assert(map.find_order("noKey") == map.order().end());
        std::cout << "Test 3.1 passed: find_order by key." << std::endl;
    }

    // ===== 4. find_key =====
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("theKey", TestA{});
        assert(map.find_key(a) == "theKey");
        assert(map.find_key("theKey") == "theKey");
        assert(map.find_key("noKey") == "");
        std::cout << "Test 4.1 passed: find_key." << std::endl;
    }

    // ===== 5. find_type_index =====
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("a", TestA{});
        auto* b = map.push_back<TestB>("b", TestB{});
        assert(map.find_type_index(a) == std::type_index(typeid(TestA)));
        assert(map.find_type_index(b) == std::type_index(typeid(TestB)));
        std::cout << "Test 5.1 passed: find_type_index." << std::endl;
    }

    // ===== 6. at / operator[] =====
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("a", TestA{42});
        assert(map.at<TestA>("a").value == 42);
        const auto& cmap = map;
        assert(cmap.at<TestA>("a").value == 42);
        // operator[]
        const TestBase* ptr = cmap["a"];
        assert(ptr != nullptr);
        assert(ptr->name() == "A");
        // 不存在的 key 抛异常
        bool caught = false;
        try { cmap["noKey"]; } catch (const std::runtime_error&) { caught = true; }
        assert(caught);
        std::cout << "Test 6.1 passed: at / operator[]." << std::endl;
    }

    // ===== 7. get / operator() =====
    {
        VarianTmap<TestBase> map;
        // get 找不到自动创建
        TestA& a = map.get<TestA>("autoA");
        assert(map.size() == 1);
        assert(a.value == 0);
        // operator() 找不到用默认值
        map("defaultB", TestB{"default"});
        assert(map.size() == 2);
        TestB* b = map.find<TestB>("defaultB");
        assert(b != nullptr);
        assert(b->text == "default");
        std::cout << "Test 7.1 passed: get / operator()." << std::endl;
    }

    // ===== 8. 遍历 =====
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("a", TestA{1});
        map.push_back<TestB>("b", TestB{"hello"});
        // order 遍历
        int count = 0;
        for (const auto* ptr : map.order()) {
            (void)ptr;
            count++;
        }
        assert(count == 2);
        // iterate 类型遍历
        for (auto& elem : map.iterate<TestA>()) {
            assert(elem.name() == "A");
        }
        // riterate
        count = 0;
        for (const auto* ptr : map.riterate()) {
            (void)ptr;
            count++;
        }
        assert(count == 2);
        std::cout << "Test 8.1 passed: iteration." << std::endl;
    }

    // ===== 9. merge =====
    {
        VarianTmap<TestBase> map1;
        map1.push_back<TestA>("m1", TestA{1});
        VarianTmap<TestBase> map2;
        map2.push_back<TestB>("m2", TestB{"merged"});
        map1.merge(map2);
        assert(map1.size() == 2);
        assert(map1.find<TestA>("m1") != nullptr);
        assert(map1.find<TestB>("m2") != nullptr);
        std::cout << "Test 9.1 passed: merge." << std::endl;
    }

    // ===== 10. 拷贝/移动 =====
    {
        VarianTmap<TestBase> map1;
        auto* a1 = map1.push_back<TestA>("a", TestA{10});
        auto* b1 = map1.push_back<TestB>("b", TestB{"copy"});
        assert(map1.size() == 2);

        // 拷贝构造 — 新 map 中的指针应与原 map 不同
        VarianTmap<TestBase> map2(map1);
        assert(map2.size() == 2);
        auto* a2 = map2.find<TestA>("a");
        auto* b2 = map2.find<TestB>("b");
        assert(a2 != nullptr);
        assert(b2 != nullptr);
        assert(a2 != a1); // 深拷贝，地址不同
        assert(b2 != b1);
        assert(a2->value == 10);  // 值相同
        assert(b2->text == "copy");

        // 拷贝赋值
        VarianTmap<TestBase> map3;
        map3 = map1;
        assert(map3.size() == 2);
        auto* a3 = map3.find<TestA>("a");
        assert(a3 != nullptr);
        assert(a3 != a1); // 深拷贝
        assert(a3->value == 10);

        // 移动构造 — 指针应保持不变
        auto* a1_addr = a1;
        auto* b1_addr = b1;
        VarianTmap<TestBase> map4(std::move(map1));
        assert(map4.size() == 2);
        assert(map1.size() == 0);        // 源被清空
        assert(map1.find("a") == nullptr);
        auto* a4 = map4.find<TestA>("a");
        auto* b4 = map4.find<TestB>("b");
        assert(a4 == a1_addr);           // 移动后指针不变
        assert(b4 == b1_addr);
        assert(a4->value == 10);         // 值不变
        assert(b4->text == "copy");

        // 移动赋值
        VarianTmap<TestBase> map5;
        map5 = std::move(map4);
        assert(map5.size() == 2);
        assert(map4.size() == 0);
        auto* a5 = map5.find<TestA>("a");
        assert(a5 == a1_addr);           // 指针不变
        assert(a5->value == 10);

        std::cout << "Test 10.1 passed: copy/move deep copy + pointer stability." << std::endl;
    }

    // ===== 11. clear =====
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("a", TestA{});
        map.push_back<TestB>("b", TestB{});
        map.clear();
        assert(map.size() == 0);
        assert(map.empty());
        // 清空后可以复用 key
        map.push_back<TestA>("a", TestA{});
        assert(map.size() == 1);
        std::cout << "Test 11.1 passed: clear()." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("a", TestA{1});
        map.push_back<TestB>("b", TestB{"b"});
        map.clear<TestA>();
        assert(map.find<TestA>("a") == nullptr);
        assert(map.find<TestB>("b") != nullptr);
        assert(map.size() == 1);
        std::cout << "Test 11.2 passed: clear<T>()." << std::endl;
    }

    // ===== 12. isTypeRegistered =====
    {
        VarianTmap<TestBase> map;
        assert(!map.isTypeRegistered<TestA>());
        assert(!map.isTypeRegistered(std::type_index(typeid(TestA))));
        map.push_back<TestA>("a", TestA{});
        assert(map.isTypeRegistered<TestA>());
        assert(map.isTypeRegistered(std::type_index(typeid(TestA))));
        std::cout << "Test 12.1 passed: isTypeRegistered." << std::endl;
    }

    // ===== 13. rename =====
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("oldName", TestA{7});
        // 通过 key 重命名
        map.rename("oldName", "newName");
        assert(map.find("oldName") == nullptr);
        assert(map.find("newName") == a);
        assert(map.find_key(a) == "newName");
        std::cout << "Test 13.1 passed: rename by key." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("oldName", TestA{7});
        auto* b = map.push_back<TestB>("other", TestB{});
        // 通过指针重命名
        map.rename(a, "newName");
        assert(map.find("oldName") == nullptr);
        assert(map.find("newName") == a);
        assert(map.find_key(a) == "newName");
        // 重命名到已存在的 key 应报错
        bool caught = false;
        try { map.rename(b, "newName"); } catch (const std::runtime_error&) { caught = true; }
        assert(caught);
        std::cout << "Test 13.2 passed: rename by pointer + duplicate check." << std::endl;
    }
    {
        // 通过 order().end() 重命名（无 key 元素）
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("hasKey", TestA{1});
        auto* b = map.push_back(TestA{2}); // 无 key
        map.rename(b, "newKey");
        assert(map.find("newKey") == b);
        assert(map.find_key(b) == "newKey");
        std::cout << "Test 13.3 passed: rename element without key." << std::endl;
    }

    // ===== 14. erase =====
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("eraseMe", TestA{42});
        map.push_back<TestB>("keep", TestB{"keep"});
        assert(map.size() == 2);
        // 通过 key 删除
        map.erase("eraseMe");
        assert(map.size() == 1);
        assert(map.find("eraseMe") == nullptr);
        assert(map.find("keep") != nullptr);
        // 删除后可以复用 key
        auto* a2 = map.push_back<TestA>("eraseMe", TestA{99});
        assert(a2 != nullptr);
        assert(a2->value == 99);
        std::cout << "Test 14.1 passed: erase by key." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("del", TestA{1});
        auto* b = map.push_back(TestB{}); // 无 key
        // 通过指针删除有 key 元素
        map.erase(a);
        assert(map.size() == 1);
        assert(map.find("del") == nullptr);
        // 通过指针删除无 key 元素
        map.erase(b);
        assert(map.size() == 0);
        std::cout << "Test 14.2 passed: erase by pointer." << std::endl;
    }
    {
        // 删除不存在的 key 应报错
        VarianTmap<TestBase> map;
        bool caught = false;
        try { map.erase("noKey"); } catch (const std::runtime_error&) { caught = true; }
        assert(caught);
        std::cout << "Test 14.3 passed: erase non-existent key throws." << std::endl;
    }

    // ===== 15. extract =====
    {
        VarianTmap<TestBase> map;
        map.push_back<TestA>("a", TestA{1});
        map.push_back<TestA>("b", TestA{2});
        map.push_back<TestA>("c", TestA{3});
        assert(map.size() == 3);
        // 从 "a" 开始提取 2 个
        auto result = map.extract("a", 2);
        assert(result.size() == 2);
        assert(map.size() == 1);
        assert(map.find("a") == nullptr);
        assert(map.find("b") == nullptr);
        assert(map.find("c") != nullptr);
        std::cout << "Test 15.1 passed: extract by key with count." << std::endl;
    }
    {
        VarianTmap<TestBase> map;
        auto* a = map.push_back<TestA>("a", TestA{1});
        auto* b = map.push_back<TestA>("b", TestA{2});
        // 通过指针提取 2 个
        auto result = map.extract(a, 2);
        assert(result.size() == 2);
        assert(map.size() == 0);
        std::cout << "Test 15.2 passed: extract by pointer with count." << std::endl;
    }
    {
        // extract count 超出时只提取到末尾
        VarianTmap<TestBase> map;
        map.push_back<TestA>("a", TestA{1});
        map.push_back<TestA>("b", TestA{2});
        auto result = map.extract("a", 10);
        assert(result.size() == 2);
        assert(map.size() == 0);
        std::cout << "Test 15.3 passed: extract with count exceeding size." << std::endl;
    }

    // ===== 16. extract + merge =====
    {
        VarianTmap<TestBase> map1;
        map1.push_back<TestA>("a1", TestA{1});
        map1.push_back<TestA>("a2", TestA{2});
        map1.push_back<TestB>("b1", TestB{"b1"});
        map1.push_back<TestB>("b2", TestB{"b2"});
        assert(map1.size() == 4);

        // 提取前两个元素
        auto extracted = map1.extract("a1", 2);
        assert(extracted.size() == 2);
        assert(map1.size() == 2);

        // 合并回来
        map1.merge(extracted);
        assert(map1.size() == 4);
        assert(map1.find<TestA>("a1") != nullptr);
        assert(map1.find<TestA>("a2") != nullptr);
        assert(map1.find<TestB>("b1") != nullptr);
        assert(map1.find<TestB>("b2") != nullptr);
        std::cout << "Test 16.1 passed: extract + merge round-trip." << std::endl;
    }
    {
        // extract 后 merge 到另一个容器
        VarianTmap<TestBase> map1;
        map1.push_back<TestA>("src", TestA{42});
        VarianTmap<TestBase> map2;
        map2.push_back<TestB>("dst", TestB{"dst"});

        auto extracted = map1.extract("src", 1);
        map2.merge(extracted);
        assert(map2.size() == 2);
        assert(map1.size() == 0);
        assert(map2.find<TestA>("src") != nullptr);
        assert(map2.find<TestB>("dst") != nullptr);
        std::cout << "Test 16.2 passed: extract from one, merge into another." << std::endl;
    }
    {
        // extract 后 merge 到指定位置
        VarianTmap<TestBase> map1;
        map1.push_back<TestA>("a", TestA{1});
        map1.push_back<TestB>("b", TestB{"b"});
        auto* a = map1.find<TestA>("a");

        auto extracted = map1.extract("a", 1);
        VarianTmap<TestBase> map2;
        map2.push_back<TestB>("first", TestB{"first"});
        map2.push_back<TestB>("last", TestB{"last"});
        // merge 到 first 之前
        map2.merge(map2.find_order("first"), extracted);
        // first 应该在 a 之后
        auto it = map2.find_order("first");
        assert(it != map2.order().begin());
        std::cout << "Test 16.3 passed: extract + merge at position." << std::endl;
    }

    std::cout << "All tests passed!" << std::endl;
    return 0;
}