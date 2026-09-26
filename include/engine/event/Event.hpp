#pragma once

#include <any>
#include <deque>
#include <optional>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

struct EventBase {};

template<typename T>
inline constexpr bool isEventType = std::is_base_of_v<EventBase, T>;

class Event {
	friend class EventQueue;
private:
	std::type_index Type;
	std::any Data;

	Event(std::type_index Type, std::any Data) : Type(std::move(Type)), Data(std::move(Data)) {}
public:
	template<typename T, typename = std::enable_if_t<isEventType<T>>>
	const T* getIf() const {
		if (std::type_index(typeid(T)) == Type)
			return std::any_cast<T>(&Data);
		return nullptr;
	}

	template<typename T>
	bool is() const {
		return std::type_index(typeid(T)) == Type;
	}
};

class EventQueue {
	friend class Event;
private:
	std::deque<std::type_index> Order;
	std::unordered_map<std::type_index, std::any> DataContainer;

	// 类型管理
	class PublicTypeOperationStruct {
		friend class EventQueue;

		class OperationStruct {
			friend class EventQueue;
			using ExtractHelperFunc = Event(*)(std::any&);

			ExtractHelperFunc ExtractHelper = nullptr;
		public:
			OperationStruct() = default;
		};

		template<typename T>
		static Event extractHelperImpl(std::any& Data) {
			auto& DataQueue = std::any_cast<std::deque<T>&>(Data);
			Event event(std::type_index(typeid(T)), std::move(DataQueue.front()));
			DataQueue.pop_front();
			return event;
		}
	public:
		std::unordered_map<std::type_index, OperationStruct> Operation;

		template<typename T>
		void registerType() {
			const std::type_index TypeIndex(typeid(T));
			if (Operation.find(TypeIndex) == Operation.end()) {
				auto& op = Operation[TypeIndex];
				op.ExtractHelper = &extractHelperImpl<T>;
			}
		}
	};

	inline static PublicTypeOperationStruct publicTypeOperation;

	template<typename T>
	std::type_index ensureTypeRegistered() {
		static_assert(isEventType<T>);
		const std::type_index TypeIndex(typeid(T));
		if (DataContainer.find(TypeIndex) == DataContainer.end()) {
			publicTypeOperation.template registerType<T>();
			std::any NewData = std::deque<T>{};
			DataContainer.emplace(TypeIndex, std::move(NewData));
		}
		return TypeIndex;
	}

	template<typename Rollback>
	class RollbackGuard {
		Rollback rollback;
		bool active = true;
	public:
		explicit RollbackGuard(Rollback&& rollback) : rollback(std::move(rollback)) {}
		RollbackGuard(const RollbackGuard&) = delete;
		RollbackGuard& operator=(const RollbackGuard&) = delete;

		~RollbackGuard() noexcept {
			if (active)
				rollback();
		}

		void release() noexcept {
			active = false;
		}
	};

	template<typename Rollback>
	static auto makeRollbackGuard(Rollback&& rollback) {
		return RollbackGuard<std::decay_t<Rollback>>(std::forward<Rollback>(rollback));
	}

	// 添加数据
	template<typename T, typename InsertDataFunc, typename = std::enable_if_t<isEventType<T>>>
	inline void insertHelper(InsertDataFunc&& insertDataFunc) {
		const std::type_index TypeIndex = ensureTypeRegistered<T>();
		auto& DataQueue = std::any_cast<std::deque<T>&>(DataContainer.at(TypeIndex));
		insertDataFunc(DataQueue);
		auto DataRollback = makeRollbackGuard([&]() noexcept { DataQueue.pop_back(); });
		Order.push_back(TypeIndex);
		DataRollback.release();
	}
public:
	template<typename U = void, typename V>
	void push(V&& value) {
		using T = std::conditional_t<std::is_same_v<U, void>, std::remove_cvref_t<V>, U>;
		insertHelper<T>(
			[&](std::deque<T>& DataQueue) {
				DataQueue.push_back(std::forward<V>(value));
			}
		);
	}

	template<typename T, typename... Args>
	void emplace(Args&&... args) {
		insertHelper<T>(
			[&](std::deque<T>& DataQueue) {
				DataQueue.emplace_back(std::forward<Args>(args)...);
			}
		);
	}

	// 访问数据
	std::optional<Event> pollEvent() {
		if (Order.empty())
			return std::nullopt;

		const std::type_index TypeIndex = Order.front();
		Event event = publicTypeOperation.Operation.at(TypeIndex).ExtractHelper(DataContainer.at(TypeIndex));
		Order.pop_front();
		return event;
	}

	// 其他函数
	size_t size() const {
		return Order.size();
	}

	bool empty() const {
		return Order.empty();
	}

	void clear() {
		Order.clear();
		DataContainer.clear();
	}
};
