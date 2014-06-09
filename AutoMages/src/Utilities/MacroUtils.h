/*
 * MacroUtils.h
 *
 *  Created on: Oct 27, 2012
 *      Author: manicqin
 */

#ifndef MACROUTILS_H_
#define MACROUTILS_H_

#define NameProperty(x)	m ## x
#define GetProperty(x)	get ## x
#define SetProperty(x)	set ## x

#define DeclareProperty(Type, x)	protected: Type NameProperty(x); \
	public: Type GetProperty(x)() const { return NameProperty(x);} \
	public: void SetProperty(x)(Type const& value){ NameProperty(x) = value;}

template<class T>
void nullifier(T& data);

namespace detail{



template<class T,bool b>
void nullifier_str_imp(T& data,const std::integral_constant<bool, b>&)
{

}

template<class T>
void nullifier_str_imp(T& data,const std::true_type&)
{
	data = "";
}

template<class T,bool b>
void nullifier_pod_imp(T& data,const std::integral_constant<bool, b>&)
{
	typedef std::integral_constant<bool,std::is_same<T,std::string>::value> truth_type;
	detail::nullifier_str_imp(data, truth_type());
}

template<class T>
void nullifier_pod_imp(T& data,const std::true_type&)
{
	data = static_cast<T>(0);
}

}

template<class T>
void nullifier(T& data)
{
	typedef std::integral_constant<bool,std::is_pod<T>::value && !std::is_array<T>::value> truth_type;
	detail::nullifier_pod_imp(data, truth_type());
}

#endif /* MACROUTILS_H_ */
