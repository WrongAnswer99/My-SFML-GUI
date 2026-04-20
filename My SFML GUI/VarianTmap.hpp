#pragma once
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <any>
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
		this->TypeIndexMap = other.TypeIndexMap;
		this->Type.resize(other.Type.size());
		std::unordered_map<Base*, Base*> PointerMap{};
		for (size_t TypeIndex = 0; TypeIndex < this->Type.size(); TypeIndex++) {
			this->Type[TypeIndex].TypeOperationIndex = other.Type[TypeIndex].TypeOperationIndex;
			TypeOperation.Operation[this->Type[TypeIndex].TypeOperationIndex].CopyHelper(this->pool, this->Type[TypeIndex].Data, other.Type[TypeIndex].Data, this->DataFinder, other.DataFinder, PointerMap);
			for (auto& elem : other.Type[TypeIndex].Key) {
				this->Type[TypeIndex].Key.emplace(elem.first, PointerMap.at(elem.second));
			}
		}
		for (auto& elem : other.Order) {
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
		this->TypeIndexMap = std::move(other.TypeIndexMap);
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
	std::pmr::unsynchronized_pool_resource pool;
	class auto_cast_pointer {
		friend class VarianTmap;
	private:
		Base* pointer = nullptr;//数据指针，即数据池中的地址
		std::pmr::list<Base*>::const_iterator orderIter;
		//特别地，把std::pmr::list<Base*>::const_iterator单独列出来，是因为insert时可能传入合法的x.order().end()，而x.order().end()无法直接解引用，需要后续判断使用场景后再决定
	public:
		template<
			typename T,
			typename = std::enable_if_t<
			VarianTmap<Base>::template isDerivedType<T>
			>
		>
		auto_cast_pointer(T* pointer) :pointer(static_cast<Base*>(pointer)) {}

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



	std::unordered_map<std::type_index, size_t>TypeIndexMap;
	std::pmr::list<Base*> Order;

	class DataPointerStruct {
		friend class VarianTmap;
	private:
		size_t TypeIndex;
		std::string Key;
		std::any/*store std::pmr::list<T>::iterator in std::any*/ Data;
		std::pmr::list<Base*>::iterator Order;
		DataPointerStruct(size_t TypeIndex, std::string Key, std::any Data, std::pmr::list<Base*>::iterator Order) :
			TypeIndex(TypeIndex), Key(std::move(Key)), Data(std::move(Data)), Order(std::move(Order)) {
		};
		DataPointerStruct(size_t TypeIndex, std::string Key, std::any Data) :
			TypeIndex(TypeIndex), Key(std::move(Key)), Data(std::move(Data)) {
		};
	public:
	};
	std::unordered_map<Base*, DataPointerStruct>DataFinder;
	class TypeStruct {
		friend class VarianTmap;
	private:
		std::map<std::string, Base*> Key;
		std::any/*store std::shared_ptr<std::pmr::list<T>> in std::any*/ Data;
		size_t TypeOperationIndex{};
	public:
		TypeStruct() = default;
	};
	std::vector<TypeStruct>Type;



	//类型管理

	class TypeOperationStruct {
		friend class VarianTmap;
		std::unordered_map<std::type_index, size_t>TypeIndexMap;
		class OperationStruct {
			friend class VarianTmap;
			std::function<void(std::any&, std::any&)>Destructor;
			std::function<void(std::pmr::unsynchronized_pool_resource&, std::any&, const std::any&, std::unordered_map<Base*, DataPointerStruct>&, const std::unordered_map<Base*, DataPointerStruct>&, std::unordered_map<Base*, Base*>&)>CopyHelper;
			std::function<void(std::pmr::unsynchronized_pool_resource&, std::any&, const std::any&, std::unordered_map<Base*, DataPointerStruct>&, const std::unordered_map<Base*, DataPointerStruct>&, std::unordered_map<Base*, Base*>&, size_t, bool)>MergeHelper;
		public:
			OperationStruct() = default;
		};
	public:
		std::vector<OperationStruct>Operation;
		template<typename T>
		size_t registerType() {
			auto iter = TypeIndexMap.find(std::type_index(typeid(T)));
			if (iter == TypeIndexMap.end()) {
				const size_t TypeIndex = TypeIndexMap.size();
				TypeIndexMap[std::type_index(typeid(T))] = TypeIndex;
				Operation.emplace_back();
				Operation.back().Destructor = [](std::any& Data, std::any& Iter) {
					auto& DataList = *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Data);
					DataList.erase(std::any_cast<typename std::pmr::list<T>::iterator&>(Iter));
					return;
					};
				Operation.back().CopyHelper = [](std::pmr::unsynchronized_pool_resource& pool, std::any& Data, const std::any& OtherData, std::unordered_map<Base*, DataPointerStruct>& Finder, const std::unordered_map<Base*, DataPointerStruct>& OtherFinder, std::unordered_map<Base*, Base*>& PointerMap) {
					const auto& OtherDataList = *std::any_cast<const std::shared_ptr<std::pmr::list<T>>&>(OtherData);
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
						Finder.emplace(NewBasePointer, DataPointerStruct(DataPointer.TypeIndex, DataPointer.Key, DataIter));
						PointerMap.emplace(BasePointer, NewBasePointer);
					}
					};
				Operation.back().MergeHelper = [](std::pmr::unsynchronized_pool_resource& pool, std::any& Data, const std::any& OtherData, std::unordered_map<Base*, DataPointerStruct>& Finder, const std::unordered_map<Base*, DataPointerStruct>& OtherFinder, std::unordered_map<Base*, Base*>& PointerMap, size_t NewTypeIndex, bool CreateNew) {
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
						Finder.emplace(NewBasePointer, DataPointerStruct(NewTypeIndex, DataPointer.Key, DataIter));
						PointerMap.emplace(BasePointer, NewBasePointer);
					}
					};
				return TypeIndex;
			}
			else return iter->second;
		}
	};
	inline static TypeOperationStruct TypeOperation;
	template<typename T>
	size_t getTypeIndexAutoCreate() {
		static_assert(VarianTmap<Base>::template isDerivedType<T>);
		auto iter = TypeIndexMap.find(std::type_index(typeid(T)));
		if (iter == TypeIndexMap.end()) {
			const size_t TypeIndex = TypeIndexMap.size();
			TypeIndexMap.emplace(std::type_index(typeid(T)), TypeIndex);
			Type.emplace_back();
			Type.back().Data = std::make_shared<std::pmr::list<T>>(
				std::pmr::polymorphic_allocator<T>(static_cast<std::pmr::memory_resource*>(&pool))
			);
			Type.back().TypeOperationIndex = TypeOperation.registerType<T>();
			return TypeIndex;
		}
		else return iter->second;
	}
	template<typename T>
	std::optional<size_t> getTypeIndex() const {
		static_assert(VarianTmap<Base>::template isDerivedType<T>);
		auto ti = std::type_index(typeid(T));
		auto iter = TypeIndexMap.find(ti);
		if (iter == TypeIndexMap.end()) {
			return std::nullopt;
		}
		else return iter->second;
	}
	template<typename T>
	static constexpr bool isDerivedType = std::is_base_of_v<Base, T> || std::is_same_v<Base, void>;



	//插入、添加数据

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
		const size_t TypeIndex = getTypeIndexAutoCreate<T>();
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
		for (const auto& elem : other.TypeIndexMap) {
			const size_t& TypeIndex = elem.second;
			size_t NewTypeIndex = 0;
			for (; NewTypeIndex < this->Type.size(); NewTypeIndex++) {
				if (this->Type[NewTypeIndex].TypeOperationIndex == other.Type[TypeIndex].TypeOperationIndex) {
					TypeOperation.Operation[other.Type[TypeIndex].TypeOperationIndex].MergeHelper(this->pool, this->Type[NewTypeIndex].Data, other.Type[TypeIndex].Data, this->DataFinder, other.DataFinder, PointerMap, NewTypeIndex, false);
					for (auto& elem : other.Type[TypeIndex].Key) {
						if (this->Type[NewTypeIndex].Key.count(elem.first)) {
							std::cerr << "[VarianTmap::merge] Key already exists." << std::endl << "  Key: " << elem.first << std::endl;
							throw std::runtime_error("[VarianTmap::merge] Key already exists.\n  Key: "s + elem.first + "\n");
						}
						this->Type[NewTypeIndex].Key.emplace(elem.first, PointerMap.at(elem.second));
					}
					goto typefound;
				}
			}
			NewTypeIndex = TypeIndexMap.size();
			this->TypeIndexMap.emplace(elem.first, NewTypeIndex);
			Type.emplace_back();
			this->Type[NewTypeIndex].TypeOperationIndex = other.Type[TypeIndex].TypeOperationIndex;
			TypeOperation.Operation[other.Type[TypeIndex].TypeOperationIndex].MergeHelper(this->pool, this->Type[NewTypeIndex].Data, other.Type[TypeIndex].Data, this->DataFinder, other.DataFinder, PointerMap, NewTypeIndex, true);
			for (auto& elem : other.Type[TypeIndex].Key) {
				this->Type[NewTypeIndex].Key.emplace(elem.first, PointerMap.at(elem.second));
			}
		typefound:;
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
		using T = std::conditional_t<std::is_same_v<U, void>, std::remove_cvref_t<V>, U>;
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
		const size_t TypeIndex = getTypeIndexAutoCreate<T>();
		return *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
	}
	template<typename T>
	const std::pmr::list<T>& iterate() const {
		auto TypeIndexOptional = getTypeIndex<T>();
		if (!TypeIndexOptional.has_value()) {
			std::cerr
				<< "[VarianTmap::iterate(const)] Type not found." << std::endl
				<< "  Type: " << typeid(T).name() << std::endl;
			throw std::runtime_error("[VarianTmap::rename_named] Type not found.\n  Type: "s + typeid(T).name() + "\n");
		}
		const size_t TypeIndex = *TypeIndexOptional;
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
		auto TypeIndexOptional = getTypeIndex<T>();
		if (!TypeIndexOptional.has_value())
			return nullptr;
		const size_t TypeIndex = *TypeIndexOptional;
		auto KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
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
		auto TypeIndexOptional = getTypeIndex<T>();
		if (!TypeIndexOptional.has_value())
			return nullptr;
		const size_t TypeIndex = *TypeIndexOptional;
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
		auto TypeIndexOptional = getTypeIndex<T>();
		if (!TypeIndexOptional.has_value())
			return Order.end();
		const size_t TypeIndex = *TypeIndexOptional;
		auto KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
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
			if (Pointer.orderIter == Order.end())goto notfound;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound;
		}
		else {
			const DataPointerStruct& DataPointer = iter->second;
			return DataPointer.Key;
		}
	notfound:;
		std::cerr
			<< "[VarianTmap::find_key] Data not found." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::find_key] Data not found.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}


	//重命名

	template<typename T>
	void rename_named(const std::string& key, const std::string& NewKey) {
		auto TypeIndexOptional = getTypeIndex<T>();
		typename std::map<std::string, Base*>::iterator KeyFindIter;
		size_t TypeIndex;
		if (!TypeIndexOptional.has_value()) {
			goto notfound;
		}
		TypeIndex = *TypeIndexOptional;
		KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
			goto notfound;
		}
		else {
			if (Type[TypeIndex].Key.count(NewKey))
				goto alreadyexist;
			Base* BasePointer = KeyFindIter->second;
			DataPointerStruct& DataPointer = DataFinder.at(BasePointer);
			DataPointer.Key = NewKey;
			auto node = Type[TypeIndex].Key.extract(KeyFindIter);
			node.key() = NewKey;
			Type[TypeIndex].Key.insert(std::move(node));
			return;
		}
	notfound:;
		std::cerr
			<< "[VarianTmap::rename_named] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename_named] Data not found.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n  New Key: " + NewKey + "\n");
	alreadyexist:;
		std::cerr
			<< "[VarianTmap::rename_named] New key already exists." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename_named] New key already exists.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n  New Key: " + NewKey + "\n");
	}
	template<typename T>
	void rename(auto_cast_pointer Pointer, const std::string& NewKey) {
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound;
			Pointer.pointer = *Pointer.orderIter;
		}
		auto TypeIndexOptional = getTypeIndex<T>();
		size_t TypeIndex;
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (!TypeIndexOptional.has_value()) {
			goto notfound;
		}
		TypeIndex = *TypeIndexOptional;
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			if (DataPointer.TypeIndex != TypeIndex) {
				goto notfound;
			}
			if (Type[TypeIndex].Key.count(NewKey))
				goto alreadyexist;
			auto node = Type[TypeIndex].Key.extract(DataPointer.Key);
			node.key() = NewKey;
			Type[TypeIndex].Key.insert(std::move(node));
			DataPointer.Key = NewKey;
			return;
		}
	notfound:;
		std::cerr
			<< "[VarianTmap::rename] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] Data not found.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	alreadyexist:;
		std::cerr
			<< "[VarianTmap::rename] New key already exists." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] New key already exists.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	}
	void rename(auto_cast_pointer Pointer, const std::string& NewKey) {
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound;
			Pointer.pointer = *Pointer.orderIter;
		}
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			size_t TypeIndex = DataPointer.TypeIndex;
			if (Type[TypeIndex].Key.count(NewKey))
				goto alreadyexist;
			Base* BasePointer = Pointer.pointer;
			auto node = Type[TypeIndex].Key.extract(DataPointer.Key);
			node.key() = NewKey;
			Type[TypeIndex].Key.insert(std::move(node));
			DataPointer.Key = NewKey;
			return;
		}
	notfound:;
		std::cerr
			<< "[VarianTmap::rename] Data not found." << std::endl
			<< "  Type: Unknown" << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl
			<< "  New Key: " << NewKey << std::endl;
		throw std::runtime_error("[VarianTmap::rename] Data not found.\n  Type: Unknown\n  Pointer: "s + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n  New Key: " + NewKey + "\n");
	alreadyexist:;
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
		return const_cast<T&>(std::as_const(*this).at<T>(key));
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
		auto TypeIndexOptional = getTypeIndex<T>();
		typename std::map<std::string, Base*>::iterator KeyFindIter;
		size_t TypeIndex;
		if (!TypeIndexOptional.has_value()) {
			goto notfound;
		}
		TypeIndex = *TypeIndexOptional;
		KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
			goto notfound;
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
	notfound:;
		std::cerr
			<< "[VarianTmap::erase_named] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Key: " << key << std::endl;
		throw std::runtime_error("[VarianTmap::erase_named] Data not found.\n  Type: "s + typeid(T).name() + "\n  Key: " + key + "\n");
	}
	template<typename T>
	void erase(auto_cast_pointer Pointer) {
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound;
			Pointer.pointer = *Pointer.orderIter;
		}
		auto TypeIndexOptional = getTypeIndex<T>();
		size_t TypeIndex;
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (!TypeIndexOptional.has_value()) {
			goto notfound;
		}
		TypeIndex = *TypeIndexOptional;
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			if (DataPointer.TypeIndex != TypeIndex) {
				goto notfound;
			}
			auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
			std::pmr::list<T>& DataList = *list_ptr;
			Base* BasePointer = Pointer.pointer;
			Order.erase(DataPointer.Order);
			Type[TypeIndex].Key.erase(DataPointer.Key);
			DataList.erase(std::any_cast<typename std::pmr::list<T>::iterator&>(DataPointer.Data));
			DataFinder.erase(iter);
			return;
		}
	notfound:;
		std::cerr
			<< "[VarianTmap::erase] Data not found." << std::endl
			<< "  Type: " << typeid(T).name() << std::endl
			<< "  Pointer: " << std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) << std::endl;
		throw std::runtime_error("[VarianTmap::erase] Data not found.\n  Type: "s + typeid(T).name() + "\n  Pointer: " + std::format("0x{:x}", reinterpret_cast<uintptr_t>(Pointer.pointer)) + "\n");
	}
	void erase(auto_cast_pointer Pointer) {
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound;
			Pointer.pointer = *Pointer.orderIter;
		}
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			size_t TypeIndex = DataPointer.TypeIndex;
			Base* BasePointer = Pointer.pointer;
			Order.erase(DataPointer.Order);
			Type[TypeIndex].Key.erase(DataPointer.Key);
			TypeOperation.Operation[Type[TypeIndex].TypeOperationIndex].Destructor(Type[TypeIndex].Data, DataPointer.Data);
			DataFinder.erase(iter);
			return;
		}
	notfound:;
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
		auto TypeIndexOptional = getTypeIndex<T>();
		typename std::map<std::string, Base*>::iterator KeyFindIter;
		size_t TypeIndex;
		if (!TypeIndexOptional.has_value()) {
			goto notfound;
		}
		TypeIndex = *TypeIndexOptional;
		KeyFindIter = Type[TypeIndex].Key.find(key);
		if (KeyFindIter == Type[TypeIndex].Key.end()) {
			goto notfound;
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
	notfound:;
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
		if (Pointer.pointer == nullptr) {
			if (Pointer.orderIter == Order.end())goto notfound;
			Pointer.pointer = *Pointer.orderIter;
		}
		auto TypeIndexOptional = getTypeIndex<T>();
		size_t TypeIndex;
		typename std::unordered_map<Base*, DataPointerStruct>::iterator iter;
		if (!TypeIndexOptional.has_value()) {
			goto notfound;
		}
		TypeIndex = *TypeIndexOptional;
		iter = DataFinder.find(Pointer.pointer);
		if (iter == DataFinder.end()) {
			goto notfound;
		}
		else {
			DataPointerStruct& DataPointer = iter->second;
			if (DataPointer.TypeIndex != TypeIndex) {
				goto notfound;
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
	notfound:;
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
		this->TypeIndexMap.clear();
		this->Order.clear();
		this->DataFinder.clear();
		this->Type.clear();
	}
	template<typename T>
	void clear() {
		auto TypeIndexOptional = getTypeIndex<T>();
		if (!TypeIndexOptional.has_value()) {
			return;
		}
		size_t TypeIndex = *TypeIndexOptional;
		Type[TypeIndex].Key.clear();
		std::pmr::list<T>& DataList = *std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
		for (auto& elem : DataList) {
			auto iter = DataFinder.find(&elem);
			Order.erase(iter->second.Order);
			DataFinder.erase(iter);
		}
		DataList.clear();
	}



	//序列化

	/*
	* TypeOrderIndex为用户定义的类型顺序编号，与内部的TypeIndex类型编号不相同
	* StreamType应为二进制流
	* 格式如下：
	* [Type0的长度(size_t)][Type0按Order顺序的值]...[TypeN的长度(size_t)][TypeN按Order顺序的值][Order的长度(size_t)][每个基类指针的TypeOrderIndex]
	* 例:
	* 当Args为Type0,Type1,Type2,Type3，但是未存储Type2，Order中有6个元素，顺序为Type1,Type0,Type3,Type0,Type1,Type1时，输出格式如下：
	* [2][Type0Data0][Type0Data1] [3][Type1Data0][Type1Data1][Type1Data2] [0] [1][Type3Data0] [6][1][0][3][0][1][1]
	* @attention 反序列化时会先读取每种类型的数据并插入到VarianTmap中，最后再根据Order的顺序将这些数据的基类指针插入到Order中，因此要求用户定义的类型顺序必须与写入时相同，否则会出现数据错乱
	*/
private:
	template<typename T, typename ...Args>
	void readHelper(auto& stream, size_t TypeOrderIndex, std::queue<Base*> DataPointerQue[]) {
		size_t TypeIndex = getTypeIndexAutoCreate<T>();
		size_t DataListSize = 0;
		stream >> DataListSize;
		std::string key{};
		T Data{};
		auto& list_ptr = std::any_cast<std::shared_ptr<std::pmr::list<T>>&>(Type[TypeIndex].Data);
		std::pmr::list<T>& DataList = *list_ptr;
		for (size_t i = 0; i < DataListSize; i++) {
			stream >> key;
			DataList.emplace_back();
			stream >> DataList.back();
			typename std::pmr::list<T>::iterator DataIter = std::prev(DataList.end());
			Base* BasePointer = static_cast<Base*>(std::addressof(*DataIter));
			DataFinder.emplace(BasePointer, DataPointerStruct(TypeIndex, key, DataIter));
			DataPointerQue[TypeOrderIndex].push(BasePointer);
			if (key != "")
				Type[TypeIndex].Key.emplace(key, BasePointer);
		}
		if constexpr (sizeof...(Args) > 0)
			readHelper<Args...>(stream, TypeOrderIndex + 1, DataPointerQue);
	}
	template<typename T, typename ...Args>
	void writeTypeOrderMapBuildHelper(size_t TypeOrderIndex, std::unordered_map<size_t, size_t>& TypeOrderMap) const {
		auto TypeIndexOptional = getTypeIndex<T>();
		if (TypeIndexOptional.has_value()) {
			const size_t TypeIndex = *TypeIndexOptional;
			TypeOrderMap.emplace(TypeIndex, TypeOrderIndex);
		}
		if constexpr (sizeof...(Args) > 0)
			writeTypeOrderMapBuildHelper<Args...>(TypeOrderIndex + 1, TypeOrderMap);
	}
	template<typename T, typename ...Args>
	void writeHelper(auto& stream, size_t TypeOrderIndex, std::queue<Base*> DataPointerQue[]) const {
		auto TypeIndexOptional = getTypeIndex<T>();
		if (TypeIndexOptional.has_value()) {
			const size_t TypeIndex = *TypeIndexOptional;
			stream << DataPointerQue[TypeOrderIndex].size();
			while (!DataPointerQue[TypeOrderIndex].empty()) {
				Base* BasePointer = DataPointerQue[TypeOrderIndex].front();
				const DataPointerStruct& DataPointer = DataFinder.at(BasePointer);
				stream << DataPointer.Key;
				stream << *static_cast<const T*>(BasePointer);//Order中的基类指针必然存在，并且已经检查过TypeIndex，可以直接static_cast
				DataPointerQue[TypeOrderIndex].pop();
			}
		}
		else {
			stream << static_cast<size_t>(0);
		}
		if constexpr (sizeof...(Args) > 0)
			writeHelper<Args...>(stream, TypeOrderIndex + 1, DataPointerQue);
	}
public:
	template<typename ...T>
	void read(auto& stream) {
		if constexpr (sizeof...(T) == 0)return;
		if (Order.size() != 0) {
			std::cerr
				<< "[VarianTmap::read] Container is NOT empty." << std::endl;
			throw std::runtime_error("[VarianTmap::read] Container is NOT empty.\n");
		}
		clear();
		std::queue<Base*>DataPointerQue[sizeof...(T)];
		readHelper<T...>(stream, 0, DataPointerQue);
		size_t OrderSize = 0;
		stream >> OrderSize;
		for (size_t i = 0; i < OrderSize; i++) {
			size_t TypeOrderIndex = 0;
			stream >> TypeOrderIndex;
			Base* BasePointer = DataPointerQue[TypeOrderIndex].front();
			DataPointerQue[TypeOrderIndex].pop();
			Order.push_back(BasePointer);
			typename std::pmr::list<Base*>::iterator OrderPointer = std::prev(Order.end());
			DataFinder.at(BasePointer).Order = OrderPointer;
		}
	}
	template<typename ...T>
	void write(auto& stream) const {
		if constexpr (sizeof...(T) == 0)return;
		std::unordered_map<size_t, size_t> TypeOrderMap;
		TypeOrderMap.reserve(sizeof...(T));
		writeTypeOrderMapBuildHelper<T...>(0, TypeOrderMap);
		std::queue<Base*>DataPointerQue[sizeof...(T)];
		std::vector<size_t>TypeOrderIndexVec{};
		for (auto& elem : Order) {
			const DataPointerStruct& DataPointer = DataFinder.at(elem);
			auto iter = TypeOrderMap.find(DataPointer.TypeIndex);
			if (iter != TypeOrderMap.end()) {
				DataPointerQue[iter->second].push(elem);
				TypeOrderIndexVec.push_back(iter->second);
			}
		}
		writeHelper<T...>(stream, 0, DataPointerQue);
		stream << TypeOrderIndexVec.size();
		for (auto& elem : TypeOrderIndexVec) {
			stream << elem;
		}
	}
};
