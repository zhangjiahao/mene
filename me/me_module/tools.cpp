/*
* tools.cpp
* Copyright(C) by MY Capital Inc. 2007-2999
*/

#include "tools.h"

//---------------------MyArray implementation-----------------------

MyArray::MyArray(size_t size, int count)
{
	m_elem_size = size;
	m_total_count = (unsigned int)count;
	m_data = malloc(count * size);
	if (m_data == NULL) {
		printf("MyArray malloc memory fail.\n");
	}
	clear();
}

MyArray::~MyArray()
{
	if (m_data != NULL){
		free(m_data);
		m_data = NULL;
	}
}

MyArray::MyArray(const MyArray *other)
{
	if (other == NULL) return;
	m_elem_size = other->m_elem_size;
	m_use_count = m_total_count = other->m_use_count;
	size_t data_len = m_use_count * m_elem_size;
	m_data = malloc(data_len);
	m_curr_pos = (char *)m_data + data_len;
	memcpy(m_data, other->m_data, data_len);
}

void MyArray::clear()
{
	m_use_count = 0;
	m_curr_pos = (char *)m_data;
	memset(m_data, 0, m_elem_size * m_total_count);
}

void  MyArray::reset()
{
	m_use_count = 0;
	m_curr_pos = (char *)m_data;
}

bool 
MyArray::expand(unsigned int new_count)
{
	if (m_total_count < new_count) {
		size_t new_size = new_count * m_elem_size;
		void* new_data = realloc(m_data, new_size);
		if (new_data == NULL){
			printf("MyArray realloc fail. new size=%lu\n", new_size);
			return false;
		}
		if (new_data != m_data){
			m_curr_pos = (char*)new_data + m_use_count * m_elem_size;
			m_data = new_data;
		}
		m_total_count = new_count;
	}
	return true;
}

bool 
MyArray::push(void * item)
{
	if (m_use_count >= m_total_count){
		if (!expand(m_total_count * 2))
			return false;
	}

	memcpy(m_curr_pos, item, m_elem_size);
	m_curr_pos += m_elem_size;
	m_use_count++;
	return true;
}

bool
MyArray::push(void * item, int num)
{
	size_t add_size = num * m_elem_size;
	if (m_use_count + num >= m_total_count){
		unsigned int need = m_total_count * 2;
		while (need < m_use_count + num){
			need *= 2;
		}
		if (!expand(need)) 
			return false;
	}

	memcpy(m_curr_pos, item, add_size);
	m_curr_pos += add_size;
	m_use_count += num;
	return true;
}

void * 
MyArray::next()
{
	if (m_use_count >= m_total_count){
		return NULL;
	}

	void* curr_pos = m_curr_pos;
	m_curr_pos += m_elem_size;
	m_use_count++;
	return curr_pos;
}

//---------------------RingQueue implementation-----------------------

RingQueue::RingQueue(int size, int count)
{
	m_elem_size = size;
	m_max_count = count;
	m_queue_data = malloc(count * size);
	if (m_queue_data == NULL) {
		printf("RingQuoue malloc memory fail.\n");
	}
	clear();
}

RingQueue::~RingQueue()
{
	if (m_queue_data != NULL){
		free(m_queue_data);
		m_queue_data = NULL;
	}
}

void  
RingQueue::clear()
{
	m_use_num = 0;
	m_real_count = 0;
	m_front_pos = m_rear_pos = (char *)m_queue_data;
	m_end_data = (char *)m_queue_data + m_max_count * m_elem_size;
	memset(m_queue_data, 0, m_max_count * m_elem_size);
}

void RingQueue::reset()
{
	m_use_num = 0;
	m_real_count = 0;
	m_front_pos = m_rear_pos = (char *)m_queue_data;
	for (int i = 0; i < m_max_count; i++){
		int* head_flag = (int *)((char *)m_rear_pos + i * m_elem_size);
		*head_flag = 0;
	}
}

void* 
RingQueue::push(void *node)
{
	void* curr_pos = m_rear_pos;
	memcpy(m_rear_pos, node, m_elem_size);
	if (m_rear_pos + m_elem_size >= m_end_data){
		m_rear_pos = (char *)m_queue_data;
	}else{
		m_rear_pos += m_elem_size;
	}
	m_real_count++;
	return curr_pos;
}

void * 
RingQueue::next()
{
	void* curr_pos = m_rear_pos;
	if (m_rear_pos + m_elem_size >= m_end_data){
		m_rear_pos = (char *)m_queue_data;
	}else{
		m_rear_pos += m_elem_size;
	}
	m_real_count++;
	return curr_pos;
}

void * 
RingQueue::pop()
{
	void* curr_pos = m_front_pos;
	if (m_front_pos + m_elem_size >= m_end_data){
		m_front_pos = (char *)m_queue_data;
	}else{
		m_front_pos += m_elem_size;
	}
	m_use_num++;
	return curr_pos;
}

void * RingQueue::get_node()
{
	int count = 0;
	while (count < m_max_count){
		int *flag = (int *)next();
		if (*flag == 0){
			return (void *)flag;
		}
		count++;
	}
	return NULL;
}
