#pragma once
#include <iostream>
namespace pavp
{
	template<typename T>
	class Queue
	{
	public:
		Queue():frontNode(nullptr), rearNode(nullptr),count(0)
		{}

		~Queue()
		{
			this->clear();
		}

		bool empty()
		{
			return !count;
		}

		size_t size()
		{
			return count;
		}

		void push(const T& node)
		{
			if (frontNode == nullptr)
				frontNode = rearNode = new QueueNode(node);
			else {
				rearNode->next = new QueueNode(node);
				rearNode = rearNode->next;
			}
			++count;
		}

		T* push()
		{
			if (frontNode == nullptr)
				frontNode = rearNode = new QueueNode();
			else {
				rearNode->next = new QueueNode();
				rearNode = rearNode->next;
			}
			++count;
			return &rearNode->data;
		}

		void pop()
		{
			if (!count)
			{
				std::cerr << "Queue pop error: queue is empty\n";
				return;
			}
			QueueNode* temp = frontNode;
			frontNode = frontNode->next;
			delete temp;
			--count;
		}

		void clear()
		{
			while (frontNode)
			{
				QueueNode* temp = frontNode;
				frontNode = frontNode->next;
				delete temp;
			}
			count = 0;
		}

		void clear(void(*process)(T*))
		{
			while (frontNode)
			{
				QueueNode* temp = frontNode;
				frontNode = frontNode->next;
				process(&temp->data);
				delete temp;
			}
			count = 0;
		}

		T front()
		{
			if (!count)
			{
				std::cerr << "Queue front error: queue is empty\n";
			}
			return frontNode->data;
		}

		T* frontp()
		{
			if (!count)
			{
				std::cerr << "Queue front error: queue is empty\n";
			}
			return &frontNode->data;
		}

	private:
		struct QueueNode
		{
			T data;
			QueueNode* next;
			QueueNode(const T& Newdata, QueueNode* nextnode = NULL) :data(Newdata), next(nextnode)
			{ }

			QueueNode():next(NULL){}
		};
		QueueNode* frontNode;
		QueueNode* rearNode;
		size_t count;
	};
}