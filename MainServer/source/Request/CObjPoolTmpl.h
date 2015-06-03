#ifndef COBJPOOLTMPL_H
#define COBJPOOLTMPL_H
#include "MemoryPool.h"
/**
 * ʵ�������ڴ�ش�������
 * Ҫ��������ȱʡ���캯��
 */
template<typename T>
class ObjectManager
{
public:
	typedef T ObjectType;

	static ObjectType* Create(MemoryPool* pool)
	{
		void* pobject=pool->Malloc(sizeof(T));
		new(pobject) ObjectType();
		return static_cast<ObjectType*>(pobject);
	}

	static ObjectType* Create(MemoryPool* pool, void* arg)
	{
		void* pobject=pool->Malloc(sizeof(T));
		new(pobject) ObjectType(arg);
		return static_cast<ObjectType*>(pobject);
	}

	static void Delete(MemoryPool* pool, ObjectType* pobject)
	{
		pobject->~ObjectType();
		pool->Free(pobject);
		pobject = NULL;
	}
};
#endif