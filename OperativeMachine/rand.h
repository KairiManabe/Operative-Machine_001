////===================================================================
////
////乱数[rand.h]
//// 
//// Author : 眞鍋快陸
//// 
////===================================================================
//#ifndef _RAND_H_			//このマクロが定義されてなかったら
//#define _RAND_H_			//2重インクルード防止のマクロを定義
//
////インクルードファイル---------------------------------------------------------
//#include <iostream>
//#include <random>
//#include "main.h"
//
//namespace std {
//	class random_device;
//}
//
//class random_device
//{
//	random_device();
//	~random_device() = default;
//
//	void operator=(const random_device&) = delete;
//	result_type operator()();
//	double entropy() const noexcept;
//	static constexpr result_type min();
//	
//	using result_type = int;
//};
//
//#endif