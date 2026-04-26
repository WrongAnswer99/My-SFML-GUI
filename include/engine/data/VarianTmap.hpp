#pragma once
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <any>
#include <memory>
#include <memory_resource>
#include <list>
#include <vector>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <string>
#include <algorithm>
#include <iostream>
#include <optional>
#include <format>
#include <ranges>
#include <functional>
#include <utility>
using namespace std::string_literals;
/*
* Varian<T> map
* 使用std::pmr::list优化，遍历效率提升，插入效率略微下降
* 注:find只搜Key,不搜索Val
*/
template<typename Base = void>
class VarianTmap {
public:
	VarianTmap() {}

	void copyHelper(const VarianTmap<Base>& other) {
		this->Type.reserve(other.Type.size());
		std::unordered_map<Base*, Base*> PointerMap{};
		for (const auto& TypeElem : other.Type) {
			publicTypeOperation.Operation[TypeElem.first].CopyMergeHelper(this->pool, this->Type[TypeElem.first].Data, TypeElem.second.Data, this->DataFinder, other.DataFinder, PointerMap, true);
			for (const auto& KeyElem : TypeElem.second.Key) {
				this->Type[TypeElem.first].Key.emplace(KeyElem.first, PointerMap.at(KeyElem.second));
			}
		}
		for (const auto& elem : other.Order) {
			Base* NewBasePointer = PointerMap.at(elem);
			this->Order.push_back(NewBasePointer);
			this->DataFinder.at(NewBasePointer).Order = std::prev(Order.end());
		}
	}

	VarianTmap(const VarianTmap<Base>& other) {
		copyHelper(other);
	}

	VarianTmap<Base>& operator=(const VarianTmap<Base>& other) {
		if (this != &other) {
			this->clear();
			copyHelper(other);
		}
		return *this;
	}

	void moveHelper(VarianTmap<Base>&& other) {
		this->Order = std::move(other.Order);
		this->DataFinder = std::move(other.DataFinder);
		this->Type = std::move(other.Type);
	}

	VarianTmap(VarianTmap<Base>&& other) noexcept {
		moveHelper(std::move(other));
	}

	VarianTmap<Base>& operator=(VarianTmap<Base>&& other) noexcept {
		if (this != &other) {
			this->clear();
			moveHelper(std::move(other));
		}
		return *this;
	}
private:
	//自动转换指针

	/*
	* 可被构造器接受的类型：
	- `Base*`
	- `Derived*`
	- `std::pmr::list<Base*>::const_iterator`
	- `std::pmr::list<Derived>::iterator`

	* 自动将上述类型转换为`Base*`以供函数统一处理，用于函数参数
	*/
	template<typename T>
	static constexpr bool isDerivedType = std::is_base_of_v<Base, T> || std::is_same_v<Base, void>;

	std::pmr::unsynchronized_pool_resource pool;

	class auto_cast_pointer {
		friend class VarianTmap;
	private:
		Base* pointer = nullptr;//数据指针，即数据池中的地址
		std::pmr::list<Base*>::const_iterator orderIter;
		//特别地，把std::pmr::list<Base*>::const_iterator单独列出来，是因为insert时可能传入合法的x.order().end()，而x.order().end()无法直接解引用，需要后续判断使用场景后再决定
	public:
		auto_cast_pointer(Base* pointer) :pointer(pointer) {}

		template<
			typename T,
			typename = std::enable_if_t<
			VarianTmap<Base>::template isDerivedType<std::remove_const_t<T>>
			>
		>
		auto_cast_pointer(T* pointer) :pointer(static_cast<Base*>(const_cast<std::remove_const_t<T>*>(pointer))) {}

		auto_cast_pointer(std::pmr::list<Base*>::const_iterator pointer) :orderIter(pointer) {}

		template<
			typename U,
			typename = std::void_t<typename std::iterator_traits<U>::value_type>,
			typename T = typename std::iterator_traits<U>::value_type,
			typename = std::enable_if_t<
			!std::is_same_v<T, Base*>&&
			std::is_same_v<typename std::pmr::list<T>::iterator, U>&&
			VarianTmap<Base>::template isDerivedType<T>
			>
		>
		auto_cast_pointer(U pointer) :pointer(static_cast<Base*>(std::addressof(*pointer))) {}
	};



	std::pmr::list<Base*> Order;

	class DataPointerStruct {
		friend class VarianTmap;
	private:
		std::type_index TypeIndex=std::type_index(typeid(void));
		std::string Key;
		std::any/*store std::pmr::list<T>::iterator in std::any*/ Data;
		std::pmr::list<Base*>::iterator Order;
	public:
		DataPointerStruct(std::type_index TypeIndex, std::string Key, std::any Data, std::pmr::list<Base*>::iterator Order) :
			TypeIndex(TypeIndex), Key(std::move(Key)), Data(std::move(Data)), Order(std::move(Order)) {
		};

		DataPointerStruct(std::type_index TypeIndex, std::string Key, std::any Data) :
			TypeIndex(TypeIndex), Key(std::move(Key)), Data(std::move(Data)) {
		};
	};

	std::unordered_map<Base*, DataPointerStruct>DataFinder;

	class TypeStruct {
		friend class VarianTmap;
	private:
		std::map<std::string, Base*> Key;
		std::any/*store std::shared_ptr<std::pmr::list<T>> in std::any*/ Data;
	public:
		TypeStruct() = default;
	};

	std::unordered_map<std::type_index,TypeStruct>Type;



	//类型管理

	class PublicTypeOperationStruct {
		friend class VarianTmap;
		
		class OperationStruct {
			friend class VarianTmap;
			std::function<void(std::any&, std::any&)>Destructor;
			std::function<void(std::pmr::unsynchronized_pool_resource&, std::any&, const std::any&, std::unordered_map<Base*, DataPointerStruct>&, const std::unordered_map<Base*, DataPointerStruct>&, std::unordered_map<Base*, Base*>&, bool)>CopyMergeHelper;
		public:
			OperationStruct() = default;
		};
	public:
		std::unordered_map<std::type_index,OperationStruct>Operation;

		template<typename T>
		void registerType() {
			std::type_index TypeIndex = std::type_index(typeid(T));
			auto iter = Operation.find(TypeIndex);
			if (iter == Operation.end()) {
				Operation[TypeIndex].Destructor = [](std::any& Data, std::any& Iter) {
					auto& DataList = *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Data);
					DataList.erase(std::any_cast<typename std::pmr::list<T>::iterator&>(Iter));
					return;
					};
				Operation[TypeIndex].CopyMergeHelper = [](std::pmr::unsynchronized_pool_resource& pool, std::any& Data, const std::any& OtherData, std::unordered_map<Base*, DataPointerStruct>& Finder, const std::unordered_map<Base*, DataPointerStruct>& OtherFinder, std::unordered_map<Base*, Base*>& PointerMap, bool CreateNew) {
					const auto& OtherDataList = *std::any_cast<const std::shared_ptr<std::pmr::list<T>>&>(OtherData);
					if (CreateNew)
						Data = std::make_shared<std::pmr::list<T>>(
							std::pmr::polymorphic_allocator<T>(static_cast<std::pmr::memory_resource*>(&pool))
						);
					auto& DataList = *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Data);
					for (auto& elem : OtherDataList) {
						DataList.push_back(elem);
						typename std::pmr::list<T>::iterator DataIter = std::prev(DataList.end());
						Base* BasePointer = const_cast<T*>(std::addressof(elem));
						Base* NewBasePointer = static_cast<Base*>(std::addressof(*DataIter));
						const DataPointerStruct& DataPointer = OtherFinder.at(BasePointer);
						Finder.emplace(NewBasePointer, DataPointerStruct(std::type_index(typeid(T)), DataPointer.Key, DataIter));
						PointerMap.emplace(BasePointer, NewBasePointer);
					}
					};
			}
		}
	};

	inline static PublicTypeOperationStruct publicTypeOperation;

	template<typename T>
	void ensureTypeRegistered() {
		static_assert(VarianTmap<Base>::template isDerivedType<T>);
		std::type_index TypeIndex = std::type_index(typeid(T));
		auto iter = Type.find(TypeIndex);
		if (iter == Type.end()) {
			publicTypeOperation.template registerType<T>();
			Type[TypeIndex].Data = std::make_shared<std::pmr::list<T>>(
				std::pmr::polymorphic_allocator<T>(static_cast<std::pmr::memory_resource*>(&pool))
			);
		}
	}

public:

	template<typename T>
	bool isTypeRegistered() const {
		std::type_index TypeIndex = std::type_index(typeid(T));
		return Type.find(TypeIndex) != Type.end();
	}

	bool isTypeRegistered(const std::type_index& TypeIndex) const {
		return Type.find(TypeIndex) != Type.end();
	}

	//插入、添加数据

private:

	void checkInsertable(auto_cast_pointer& Where) {
		if (Where.pointer != nullptr) {
			if (!DataFinder.count(Where.pointer)) {
				std::cerr << "[VarianTmap::insert] Pointer not found." << std::endl;
				throw std::runtime_error("[VarianTmap::insert] Pointer not found.\n");
			}
		}
		else {
			if (Where.orderIter != Order.end()) {
				Where.pointer = *Where.orderIter;
				if (!DataFinder.count(Where.pointer)) {
					std::cerr << "[VarianTmap::insert] Pointer not found." << std::endl;
					throw std::runtime_error("[VarianTmap::insert] Pointer not found.\n");
				}
			}
			//else pointer=nullptr; 若为Order.end()，pointer仍保持nullptr
		}
	}

	template<typename T, typename InsertDataFunc, typename InsertOrderFunc, typename = std::enable_if_t<isDerivedType<T>>>
	inline T* insertHelper(const std::string& key, InsertDataFunc&& insertDataFunc, InsertOrderFunc&& insertOrderFunc) {
		ensureTypeRegistered<T>();
		std::type_index TypeIndex = std::type_index(typeid(T));
		if (Type[TypeIndex].Key.count(key)) {
			std::cerr << "[VarianTmap::insert] Key already exists." << std::endl << "  Key: " << key << std::endl;
			throw std::runtime_error("[VarianTmap::insert] Key already exists.\n  Key: "s + key + "\n");
		}
		auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
		std::pmr::list<T>& DataList = *list_ptr;
		const typename std::pmr::list<T>::iterator DataIter = insertDataFunc(DataList);
		T* TypedPointer = std::addressof(*DataIter);
		Base* BasePointer = static_cast<Base*>(TypedPointer);
		typename std::pmr::list<Base*>::iterator OrderPointer = insertOrderFunc(BasePointer);
		DataFinder.emplace(BasePointer, DataPointerStruct(TypeIndex, key, DataIter, OrderPointer));
		if (key != "")
			Type[TypeIndex].Key.emplace(key, BasePointer);
		return TypedPointer;
	}

	void mergeHelper(const VarianTmap<Base>& other, std::unordered_map<Base*, Base*>& PointerMap) {
		for (const auto& TypeElem : other.Type) {
			const std::type_index& TypeIndex = TypeElem.first;
			bool NeedCreate = !isTypeRegistered(TypeIndex);
			publicTypeOperation.Operation[TypeIndex].CopyMergeHelper(this->pool, this->Type[TypeIndex].Data, TypeElem.second.Data, this->DataFinder, other.DataFinder, PointerMap, NeedCreate);
			for (auto& KeyElem : TypeElem.second.Key) {
				if (this->Type[TypeIndex].Key.count(KeyElem.first)) {
					std::cerr << "[VarianTmap::merge] Key already exists." << std::endl << "  Key: " << KeyElem.first << std::endl;
					throw std::runtime_error("[VarianTmap::merge] Key already exists.\n  Key: "s + KeyElem.first + "\n");
				}
				this->Type[TypeIndex].Key.emplace(KeyElem.first, PointerMap.at(KeyElem.second));
			}
		}
	}
public:

	template<typename U = void, typename V>
	auto push_back_named(const std::string& key, V&& value) {
		using T = std::conditional_t<std::is_same_v<U, void>, std::remove_cvref_t<V>, U>;
		return insertHelper<T>(
			key,
			[&](std::pmr::list<T>& DataList)->typename std::pmr::list<T>::iterator {
				DataList.push_back(std::forward<V>(value));
				return std::prev(DataList.end());
			},
			[&](Base* BasePointer)->typename std::pmr::list<Base*>::iterator {
				Order.push_back(BasePointer);
				return std::prev(Order.end());
			}
		);
	}

	template<typename U = void, typename V>
	auto push_back(V&& value) {
		return push_back_named<U>("", std::forward<V>(value));
	}

	template<typename U = void, typename V>
	auto insert_named(auto_cast_pointer Where, const std::string& key, V&& value) {
		using RawType = std::remove_cvref_t<V>;
		using T = std::conditional_t<std::is_same_v<U, void>,
			std::conditional_t<std::is_same_v<RawType, std::unique_ptr<typename std::unique_ptr<RawType>::element_type>>,
				typename std::unique_ptr<RawType>::element_type,
				std::conditional_t<std::is_same_v<RawType, std::shared_ptr<typename std::shared_ptr<RawType>::element_type>>,
					typename std::shared_ptr<RawType>::element_type,
					RawType>>,
			U>;
		return insertHelper<T>(
			key,
			[&](std::pmr::list<T>& DataList)->typename std::pmr::list<T>::iterator {
				DataList.push_back(std::forward<V>(value));
				return std::prev(DataList.end());
			},
			[&](Base* BasePointer)->typename std::pmr::list<Base*>::iterator {
				checkInsertable(Where);
				if (Where.pointer == nullptr) {
					Order.push_back(BasePointer);
					return std::prev(Order.end());
				}
				else {
					DataPointerStruct& DataPointer = DataFinder.at(Where.pointer);
					typename std::pmr::list<Base*>::iterator& WhereOrderIter = DataPointer.Order;
					return Order.insert(WhereOrderIter, BasePointer);
				}
			}
		);
	}

	template<typename U = void, typename V>
	auto insert(auto_cast_pointer Where, V&& value) {
		return insert_named<U>(Where, "", std::forward<V>(value));
	}

	template<typename U = void, typename V>
	auto push_front_named(const std::string& key, V&& value) {
		using T = std::conditional_t<std::is_same_v<U, void>, std::remove_cvref_t<V>, U>;
		return insertHelper<T>(
			key,
			[&](std::pmr::list<T>& DataList)->typename std::pmr::list<T>::iterator {
				//数据池中的数据无序，这里直接向数据池中push_back即可，顺序仅靠Order维护，不是写错
				DataList.push_back(std::forward<V>(value));
				return std::prev(DataList.end());
			},
			[&](Base* BasePointer)->typename std::pmr::list<Base*>::iterator {
				Order.push_front(BasePointer);
				return Order.begin();
			}
		);
	}

	template<typename U = void, typename V>
	auto push_front(V&& value) {
		return push_front_named<U>("", std::forward<V>(value));
	}


	template<typename T, typename... Args>
	auto emplace_named(auto_cast_pointer Where, const std::string& key, Args&&... args) {
		checkInsertable(Where);
		return insertHelper<T>(
			key,
			[&](std::pmr::list<T>& DataList)->typename std::pmr::list<T>::iterator {
				DataList.emplace_back(std::forward<Args>(args)...);
				return std::prev(DataList.end());
			},
			[&](Base* BasePointer)->typename std::pmr::list<Base*>::iterator {
				if (Where.pointer == nullptr) {
					Order.push_back(BasePointer);
					return std::prev(Order.end());
				}
				else {
					DataPointerStruct& DataPointer = DataFinder.at(Where.pointer);
					typename std::pmr::list<Base*>::iterator& WhereOrderIter = DataPointer.Order;
					return Order.insert(WhereOrderIter, BasePointer);
				}
			}
		);
	}

	template<typename T, typename... Args>
	auto emplace(auto_cast_pointer Where, Args&&... args) {
		return emplace_named<T>(Where, "", std::forward<Args>(args)...);
	}

	void merge(const VarianTmap<Base>& other) {
		std::unordered_map<Base*, Base*> PointerMap{};
		mergeHelper(other, PointerMap);
		for (auto& elem : other.Order) {
			Base* NewBasePointer = PointerMap.at(elem);
			this->Order.push_back(NewBasePointer);
			this->DataFinder.at(NewBasePointer).Order = std::prev(Order.end());
		}
	}

	void merge(auto_cast_pointer Where, const VarianTmap<Base>& other) {
		checkInsertable(Where);
		std::unordered_map<Base*, Base*> PointerMap{};
		mergeHelper(other, PointerMap);
		typename std::pmr::list<Base*>::iterator OrderIter;
		if (Where.pointer == nullptr) {
			OrderIter = Order.end();
		}
		else {
			DataPointerStruct& DataPointer = DataFinder.at(Where.pointer);
			OrderIter = DataPointer.Order;
		}
		for (auto& elem : std::views::reverse(other.Order)) {
			Base* NewBasePointer = PointerMap.at(elem);
			OrderIter = this->Order.insert(OrderIter, NewBasePointer);
			this->DataFinder.at(NewBasePointer).Order = OrderIter;
		}
	}



	//遍历管理

	const std::pmr::list<Base*>& order() const {
		return Order;
	}

	const std::pmr::list<Base*>& iterate() const {
		return Order;
	}

	const auto riterate() const {
		return std::views::reverse(Order);
	}

	template<typename T>
	std::pmr::list<T>& iterate() {
		ensureTypeRegistered<T>();
		std::type_index TypeIndex = std::type_index(typeid(T));
		return *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
	}

	template<typename T>
	const std::pmr::list<T>& iterate() const {
		if (!isTypeRegistered<T>()) {
			std::cerr
				<< "[VarianTmap::iterate(const)] Type not found." << std::endl
				<< "  Type: " << typeid(T).name() << std::endl;
			throw std::runtime_error("[VarianTmap::iterate(const)] Type not found.\n  Type: "s + typeid(T).name() + "\n");
		}
		std::type_index TypeIndex = std::type_index(typeid(T));
		return *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
	}

	typename std::pmr::list<Base*>::const_iterator begin() const {
		return Order.begin();
	}

	typename std::pmr::list<Base*>::const_iterator end() const {
		return Order.end();
	}



	//查找数据

	template<typename T>
	T* find_named(const std::string& key) const {
		if (!isTypeRegistered<T>()) {
			return nullptr;
		}
		std::type_index TypeIndex = std::type_index(typeid(T));
		auto KeyFindIter = Type.at(TypeIndex).Key.find(key);
		if (KeyFindIter == Type.at(TypeIndex).Key.end()) {
			return nullptr;
		}
		else {
			return static_cast<T*>(KeyFindIter->second);
		}
	}

	Base* find(auto_cast_pointer Pointer) const {
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())return nullptr;
			Pointer.pointer = *Pointer.orderIter;
		}
		return DataFinder.count(Pointer.pointer) ? Pointer.pointer : nullptr;
	}

	template<typename T>
	T* find(auto_cast_pointer Pointer) const {
		if (!isTypeRegistered<T>()) {
			return nullptr;
		}
		std::type_index TypeIndex = std::type_index(typeid(T));
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())return nullptr;
			Pointer.pointer = *Pointer.orderIter;
		}
		auto iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end())
			return nullptr;
		else {
			if (iter->second.TypeIndex != TypeIndex)
				return nullptr;
			return static_cast<T*>(Pointer.pointer);
		}
	}

	template<typename T>
	std::pmr::list<Base*>::const_iterator find_order_named(const std::string& key) const {
		std::type_index TypeIndex = std::type_index(typeid(T));
		if (!isTypeRegistered<T>()) {
			return Order.end();
		}
		auto KeyFindIter = Type.at(TypeIndex).Key.find(key);
		if (KeyFindIter == Type.at(TypeIndex).Key.end()) {
			return Order.end();
		}
		else {
			auto iter = DataFinder.find(KeyFindIter->second);
			return iter->second.Order;
		}
	}

	std::pmr::list<Base*>::const_iterator find_order(auto_cast_pointer Pointer) const {
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())return Order.end();
			Pointer.pointer = *Pointer.orderIter;
		}
		auto iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			return Order.end();
		}
		else {
			return iter->second.Order;
		}
	}

	const std::string& find_key(auto_cast_pointer Pointer) const {
		typename std::unordered_map<Base*, DataPointerStruct>::const_iterator iter;
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_find_key;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_find_key;
		}
		else {
			const DataPointerStruct& DataPointer = iter->second;
			return DataPointer.Key;
		}
	notfound_find_key:;
		std::cerr
			<< "[VarianTmap::find_key] Data not found." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::find_key] Data not found.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}

	std::type_index find_type_index(auto_cast_pointer Pointer) const {
		typename std::unordered_map<Base*, DataPointerStruct>::const_iterator iter;
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_find_type_index;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_find_type_index;
		}
		else {
			const DataPointerStruct& DataPointer = iter->second;
			return DataPointer.TypeIndex;
		}
	notfound_find_type_index:;
		std::cerr
			<< "[VarianTmap::find_type_index] Data not found." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::find_type_index] Data not found.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}


	//重命名

	template<typename T>
	void rename_named(const std::string& key, const std::string& NewKey) {
		std::type_index TypeIndex = std::type_index(typeid(T));
		typename std::map<std::string, Base*>::iterator KeyFindIter;
		if (!isTypeRegistered<T>()) {
			goto notfound_rename_named;
		}
		KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
			goto notfound_rename_named;
		}
		else {
			if (Type[TypeIndex].Key.count(NewKey))
				goto alreadyexist_rename_named;
			Base* BasePointer = KeyFindIter->second;
			DataPointerStruct& DataPointer = DataFinder.at(BasePointer);
			DataPointer.Key = NewKey;
			auto node = Type[TypeIndex].Key.extract(KeyFindIter);
			node.key() = NewKey;
			Type[TypeIndex].Key.insert(std::move(node));
			return;
		}
	notfound_rename_named:;
		std::cerr
			<< "[VarianTmap::rename_named] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename_named] Data not found.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n  New Key: " + NewKey + "\n");
	alreadyexist_rename_named:;
		std::cerr
			<< "[VarianTmap::rename_named] New key already exists." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename_named] New key already exists.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n  New Key: " + NewKey + "\n");
	}

	template<typename T>
	void rename(auto_cast_pointer Pointer, const std::string& NewKey) {
		std::type_index TypeIndex = std::type_index(typeid(T));
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (!isTypeRegistered<T>()) {
			goto notfound_rename_typed;
		}
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_rename_typed;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_rename_typed;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			if (DataPointer.TypeIndex != TypeIndex) {
				goto notfound_rename_typed;
			}
			if (Type[TypeIndex].Key.count(NewKey))
				goto alreadyexist_rename_typed;
			auto node = Type[TypeIndex].Key.extract(DataPointer.Key);
			node.key() = NewKey;
			Type[TypeIndex].Key.insert(std::move(node));
			DataPointer.Key = NewKey;
			return;
		}
	notfound_rename_typed:;
		std::cerr
			<< "[VarianTmap::rename] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] Data not found.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	alreadyexist_rename_typed:;
		std::cerr
			<< "[VarianTmap::rename] New key already exists." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] New key already exists.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	}

	void rename(auto_cast_pointer Pointer, const std::string& NewKey) {
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_rename_untyped;
			Pointer.pointer = *Pointer.orderIter;
		}
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_rename_untyped;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			std::type_index TypeIndex = DataPointer.TypeIndex;
			if (Type[TypeIndex].Key.count(NewKey))
				goto alreadyexist_rename_untyped;
			Base* BasePointer = Pointer.pointer;
			auto node = Type[TypeIndex].Key.extract(DataPointer.Key);
			node.key() = NewKey;
			Type[TypeIndex].Key.insert(std::move(node));
			DataPointer.Key = NewKey;
			return;
		}
	notfound_rename_untyped:;
		std::cerr
			<< "[VarianTmap::rename] Data not found." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] Data not found.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	alreadyexist_rename_untyped:;
		std::cerr
			<< "[VarianTmap::rename] New key already exists." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] New key already exists.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	}



	//访问数据
	//访问数据只能从键访问对应的值

	template<typename T>
	const T& at(const std::string& key) const {
		T* pointer = find_named<T>(key);
		if (pointer == nullptr) {
			std::cerr
				<< "[VarianTmap::at] Data not found." << std::endl
				<< "  Type: " << typeid(T).name() << std::endl
				<< "  Key: " << key << std::endl;
			throw std::runtime_error("[VarianTmap::at] Data not found.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n");
		}
		else return *pointer;
	}

	template<typename T>
	T& at(const std::string& key) {
		return const_cast<T&>(std::as_const(*this).template at<T>(key));
	}

	template<typename T>
	T& get(const std::string& key) {
		T* pointer = find_named<T>(key);
		if (pointer == nullptr)
			return *push_back_named<T>(key, T{});
		else return *pointer;
	}



	//删除数据

	template<typename T>
	void erase_named(const std::string& key) {
		std::type_index TypeIndex = std::type_index(typeid(T));
		typename std::map<std::string, Base*>::iterator KeyFindIter;
		if (!isTypeRegistered<T>()) {
			goto notfound_erase_named;
		}
		KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
			goto notfound_erase_named;
		}
		else {
			auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
			std::pmr::list<T>& DataList = *list_ptr;
			Base* BasePointer = KeyFindIter->second;
			DataPointerStruct& DataPointer = DataFinder.at(BasePointer);
			Order.erase(DataPointer.Order);
			DataList.erase(std::any_cast<typename std::pmr::list<T>::iterator&>(DataPointer.Data));
			Type[TypeIndex].Key.erase(KeyFindIter);
			DataFinder.erase(BasePointer);
			return;
		}
	notfound_erase_named:;
		std::cerr
			<< "[VarianTmap::erase_named] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl;
		throw std::runtime_error("[VarianTmap::erase_named] Data not found.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n");
	}

	template<typename T>
	void erase(auto_cast_pointer Pointer) {
		std::type_index TypeIndex = std::type_index(typeid(T));
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (!isTypeRegistered<T>()) {
			goto notfound_erase;
		}
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_erase;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_erase;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			if (DataPointer.TypeIndex != TypeIndex) {
				goto notfound_erase;
			}
			auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
			std::pmr::list<T>& DataList = *list_ptr;
			Base* BasePointer = Pointer.pointer;
			Order.erase(DataPointer.Order);
			if (DataPointer.Key != "") {
				Type[TypeIndex].Key.erase(DataPointer.Key);
			}
			DataList.erase(std::any_cast<typename std::pmr::list<T>::iterator&>(DataPointer.Data));
			DataFinder.erase(iter);
			return;
		}
	notfound_erase:;
		std::cerr
			<< "[VarianTmap::erase] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::erase] Data not found.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}

	void erase(auto_cast_pointer Pointer) {
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_erase_untyped;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_erase_untyped;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			std::type_index TypeIndex = DataPointer.TypeIndex;
			Base* BasePointer = Pointer.pointer;
			Order.erase(DataPointer.Order);
			if (DataPointer.Key != "") {
				Type[TypeIndex].Key.erase(DataPointer.Key);
			}
			publicTypeOperation.Operation[TypeIndex].Destructor(Type[TypeIndex].Data, DataPointer.Data);
			DataFinder.erase(iter);
			return;
		}
	notfound_erase_untyped:;
		std::cerr
			<< "[VarianTmap::erase] Data not found." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::erase] Data not found.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}



	//抽取数据

	/*
	* @attention 受`std::pmr::list`限制，与`std::map`、`set`或`unordered_*`的`extract`不同，该`extract`直接从容器中删去，并返回对应的值，而不是指针
	*/
	template<typename T>
	T extract_named(const std::string& key) {
		std::type_index TypeIndex = std::type_index(typeid(T));
		typename std::map<std::string, Base*>::iterator KeyFindIter;
		if (!isTypeRegistered<T>()) {
			goto notfound_extract_named;
		}
		KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
			goto notfound_extract_named;
		}
		else {
			auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
			std::pmr::list<T>& DataList = *list_ptr;
			Base* BasePointer = KeyFindIter->second;
			DataPointerStruct& DataPointer = DataFinder.at(BasePointer);
			Order.erase(DataPointer.Order);
			auto& DataIter = std::any_cast<typename std::pmr::list<T>::iterator&>(DataPointer.Data);
			T data = std::move(*DataIter);
			DataList.erase(DataIter);
			Type[TypeIndex].Key.erase(KeyFindIter);
			DataFinder.erase(BasePointer);
			return std::move(data);
		}
	notfound_extract_named:;
		std::cerr
			<< "[VarianTmap::extract_named] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl;
		throw std::runtime_error("[VarianTmap::extract_named] Data not found.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n");
	}

	/*
	* @attention 受`std::pmr::list`限制，与`std::map`、`set`或`unordered_*`的`extract`不同，该`extract`直接从容器中删去，并返回对应的值，而不是指针
	*/
	template<typename T>
	T extract(auto_cast_pointer Pointer) {
		std::type_index TypeIndex = std::type_index(typeid(T));
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound_extract;
			Pointer.pointer = *Pointer.orderIter;
		}
		if (!isTypeRegistered<T>()) {
			goto notfound_extract;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound_extract;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			if (DataPointer.TypeIndex != TypeIndex) {
				goto notfound_extract;
			}
			auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
			std::pmr::list<T>& DataList = *list_ptr;
			Base* BasePointer = Pointer.pointer;
			Order.erase(DataPointer.Order);
			Type[TypeIndex].Key.erase(DataPointer.Key);
			auto& DataIter = std::any_cast<typename std::pmr::list<T>::iterator&>(DataPointer.Data);
			T data = std::move(*DataIter);
			DataList.erase(DataIter);
			DataFinder.erase(iter);
			return std::move(data);
		}
	notfound_extract:;
		std::cerr
			<< "[VarianTmap::extract] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::extract] Data not found.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}



	//其他函数

	size_t size() const {
		return Order.size();
	}

	bool empty() const {
		return Order.size() == 0;
	}

	void clear() {
		this->Order.clear();
		this->DataFinder.clear();
		this->Type.clear();
	}
	
	template<typename T>
	void clear() {
		std::type_index TypeIndex = std::type_index(typeid(T));
		if (!isTypeRegistered<T>()) {
			return;
		}
		Type[TypeIndex].Key.clear();
		std::pmr::list<T>& DataList = *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
		for (auto& elem : DataList) {
			auto iter = DataFinder.find(&elem);
			Order.erase(iter->second.Order);
			DataFinder.erase(iter);
		}
		DataList.clear();
	}
};
