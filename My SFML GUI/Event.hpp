#pragma once
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <queue>
#include <string>
#include <any>
#include <optional>
#include <functional>
#include <array>
struct EventBase {};
class Event {
	friend class EventQueue;
private:
	std::type_index Type;
	std::any Data;
	Event(std::type_index Type, std::any Data) :Type(std::move(Type)), Data(std::move(Data)) {}
public:
	template<typename T, typename = std::enable_if_t<EventQueue::isDerivedType<T>>>
	const T* getIf() const {
		if (std::type_index(typeid(T)) == Type)
			return std::any_cast<T>(&Data);
		else return nullptr;
	}
	template<typename T>
	bool is() const {
		return std::type_index(typeid(T)) == Type;
	}
};
class EventQueue {
	friend class Event;
private:
	std::unordered_map<std::type_index, size_t>TypeIndexMap;
	std::queue<size_t>Order;

	class TypeStruct {
		friend class EventQueue;
	private:
		std::any/*store std::queue<T> in std::any*/ Data;
		size_t TypeOperationIndex{};
	public:
		TypeStruct() = default;
	};
	std::vector<TypeStruct>Type;



	//类型管理

	class TypeOperationStruct {
		friend class EventQueue;
		std::unordered_map<std::type_index, size_t>TypeIndexMap;
		class OperationStruct {
			friend class EventQueue;
			std::function<Event(std::any&)>extractHelper;
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
				Operation.back().extractHelper = [](std::any& Data)->Event {
					auto& DataQueue = std::any_cast<std::queue<T>&>(Data);
					Event event(std::type_index(typeid(T)), std::move(DataQueue.front()));
					DataQueue.pop();
					return event;
					};
				return TypeIndex;
			}
			else return iter->second;
		}
	};
	inline static TypeOperationStruct TypeOperation;
	template<typename T>
	size_t getTypeIndexAutoCreate() {
		auto iter = TypeIndexMap.find(std::type_index(typeid(T)));
		if (iter == TypeIndexMap.end()) {
			const size_t TypeIndex = TypeIndexMap.size();
			TypeIndexMap.emplace(std::type_index(typeid(T)), TypeIndex);
			Type.emplace_back();
			Type.back().Data = std::queue<T>{};
			Type.back().TypeOperationIndex = TypeOperation.registerType<T>();
			return TypeIndex;
		}
		else return iter->second;
	}
	template<typename T>
	std::optional<size_t> getTypeIndex() const {
		auto ti = std::type_index(typeid(T));
		auto iter = TypeIndexMap.find(ti);
		if (iter == TypeIndexMap.end()) {
			return std::nullopt;
		}
		else return iter->second;
	}
	template<typename T>
	static constexpr bool isDerivedType = std::is_base_of_v<EventBase, T>;



	//添加数据

	template<typename T, typename InsertDataFunc, typename = std::enable_if_t<isDerivedType<T>>>
	inline void insertHelper(InsertDataFunc&& insertDataFunc) {
		const size_t TypeIndex = getTypeIndexAutoCreate<T>();
		std::queue<T>& DataQueue = std::any_cast<std::queue<T>&>(Type[TypeIndex].Data);
		insertDataFunc(DataQueue);
		Order.push(TypeIndex);
	}
public:
	template<typename U = void, typename V>
	void push(V&& value) {
		using T = std::conditional_t<std::is_same_v<U, void>, std::remove_cvref_t<V>, U>;
		return insertHelper<T>(
			[&](std::queue<T>& DataQueue) {
				DataQueue.push(std::forward<V>(value));
			}
		);
	}
	template<typename T, typename... Args>
	auto emplace(Args&&... args) {
		return insertHelper<T>(
			[&](std::queue<T>& DataQueue) {
				DataQueue.emplace(std::forward<Args>(args)...);
			}
		);
	}



	//访问数据

	const std::optional<Event> pollEvent() {
		if (Order.empty())return std::nullopt;
		size_t TypeIndex = Order.front();
		Order.pop();
		return TypeOperation.Operation[Type[TypeIndex].TypeOperationIndex].extractHelper(Type[TypeIndex].Data);
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
		while (!Order.empty()) {
			Order.pop();
		}
		this->Type.clear();
	}
};
