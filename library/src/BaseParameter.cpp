/************************************************************************
\file BaseParameter.cpp
\brief Implementation of the Class BaseParameter
\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
\date 01-May-2015 12:00:00
************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2015, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/
#include "BaseParameter.h"

using namespace IndustrialNetwork::POWERLINK::Core::ErrorHandling;
using namespace IndustrialNetwork::POWERLINK::Core::Utilities;
using namespace IndustrialNetwork::POWERLINK::Core::CoreConfiguration;
using namespace IndustrialNetwork::POWERLINK::Core::ObjectDictionary;

namespace IndustrialNetwork
{
	namespace POWERLINK
	{
		namespace Core
		{
			namespace ObjectDictionary
			{
				BaseParameter::BaseParameter(const std::string& uniqueID, const ParameterAccess& parameterAccess, const std::string& dataTypeUniqueIDRef) :
					uniqueID(uniqueID),
					dataTypeUniqueIDRef(dataTypeUniqueIDRef),
					complexDataType(std::shared_ptr<ComplexDataType>()),
					defaultValue(boost::optional<boost::any>()),
					actualValue(boost::optional<boost::any>()),
					allowedRanges(std::vector<std::pair<boost::any, boost::any>>()),
					allowedValues(std::vector<boost::any>()),
					parameterAccess(parameterAccess),
					dataType(boost::optional<IEC_Datatype>()),
					actualValueBitset(boost::dynamic_bitset<>()),
					actualValueNotDefaultValue(false)
				{}

				BaseParameter::BaseParameter(const std::string& uniqueID, const ParameterAccess& parameterAccess, const IEC_Datatype& dataType) :
					uniqueID(uniqueID),
					dataTypeUniqueIDRef(""),
					complexDataType(std::shared_ptr<ComplexDataType>()),
					defaultValue(boost::optional<boost::any>()),
					actualValue(boost::optional<boost::any>()),
					allowedRanges(std::vector<std::pair<boost::any, boost::any>>()),
					allowedValues(std::vector<boost::any>()),
					parameterAccess(parameterAccess),
					dataType(dataType),
					actualValueBitset(boost::dynamic_bitset<>()),
					actualValueNotDefaultValue(false)
				{}

				BaseParameter::BaseParameter(const std::string& uniqueID) :
					uniqueID(uniqueID),
					dataTypeUniqueIDRef(""),
					complexDataType(std::shared_ptr<ComplexDataType>()),
					defaultValue(boost::optional<boost::any>()),
					actualValue(boost::optional<boost::any>()),
					allowedRanges(std::vector<std::pair<boost::any, boost::any>>()),
					allowedValues(std::vector<boost::any>()),
					parameterAccess(ParameterAccess::undefined),
					dataType(boost::optional<IEC_Datatype>()),
					actualValueBitset(boost::dynamic_bitset<>()),
					actualValueNotDefaultValue(false)
				{}

				BaseParameter::~BaseParameter()
				{
					this->allowedRanges.clear();
					this->allowedValues.clear();
				}

				const std::string& BaseParameter::GetUniqueID() const
				{
					return this->uniqueID;
				}

				const std::string& BaseParameter::GetDataTypeUniqueIDRef() const
				{
					return this->dataTypeUniqueIDRef;
				}

				void BaseParameter::SetDataTypeUniqueIDRef(const std::string& _dataTypeUniqueIDRef)
				{
					this->dataTypeUniqueIDRef = _dataTypeUniqueIDRef;
				}

				const std::shared_ptr<ComplexDataType>& BaseParameter::GetComplexDataType() const
				{
					return this->complexDataType;
				}

				void BaseParameter::SetComplexDataType(const std::shared_ptr<ComplexDataType>& complexType)
				{
					this->complexDataType = complexType;
				}

				const ParameterAccess& BaseParameter::GetParameterAccess() const
				{
					return this->parameterAccess;
				}

				void BaseParameter::SetParameterAccess(const ParameterAccess& _access)
				{
					this->parameterAccess = _access;
				}

				std::uint32_t BaseParameter::GetBitSize() const
				{
					if (!this->dataTypeUniqueIDRef.empty())
						return this->complexDataType->GetBitSize();
					else
						return GetIECDataTypeBitSize(this->dataType.get());
				}

				const boost::optional<boost::any>& BaseParameter::GetParameterDefaultValue() const
				{
					return this->defaultValue;
				}

				const boost::optional<boost::any>& BaseParameter::GetParameterActualValue() const
				{
					return this->actualValue;
				}

				const boost::optional<IEC_Datatype>& BaseParameter::GetDataType() const
				{
					return this->dataType;
				}

				void BaseParameter::SetDataType(const IEC_Datatype& _dataType)
				{
					this->dataType = _dataType;
				}

				bool BaseParameter::HasActualValue() const
				{
					if (this->actualValue.is_initialized())
						return true;
					else
						return false;
				}

				bool BaseParameter::HasDefaultValue() const
				{
					if (this->defaultValue.is_initialized())
						return true;
					else
						return false;
				}

				template<typename T>
				Result BaseParameter::ValidateParameterValue(T actualValueToValidate)
				{
					for (auto& allowedValue : this->allowedValues)
					{
						if (boost::any_cast<T>(allowedValue) == actualValueToValidate)
							return Result();
					}

					for (auto& range : this->allowedRanges)
					{
						//Check minimum value
						if (actualValueToValidate > boost::any_cast<T>(range.first) && actualValueToValidate < boost::any_cast<T>(range.second))
							return Result();
					}
					return Result();
				}

				template<> Result BaseParameter::ValidateParameterValue<float>(float actualValueToValidate)
				{
					for (auto& allowedValue : this->allowedValues)
					{
						if (std::fabs(boost::any_cast<float>(allowedValue) - actualValueToValidate) < std::numeric_limits<float>::epsilon())
							return Result();
					}

					for (auto& range : this->allowedRanges)
					{
						//Check minimum value
						if (actualValueToValidate > boost::any_cast<float>(range.first) && actualValueToValidate < boost::any_cast<float>(range.second))
							return Result();
					}
					return Result();
				}

				template<> Result BaseParameter::ValidateParameterValue<double>(double actualValueToValidate)
				{
					for (auto& allowedValue : this->allowedValues)
					{
						if (std::fabs(boost::any_cast<double>(allowedValue) - actualValueToValidate) < std::numeric_limits<double>::epsilon())
							return Result();
					}

					for (auto& range : this->allowedRanges)
					{
						//Check minimum value
						if (actualValueToValidate > boost::any_cast<double>(range.first) && actualValueToValidate < boost::any_cast<double>(range.second))
							return Result();
					}
					return Result();
				}

				template<> Result BaseParameter::ValidateParameterValue<std::string>(std::string actualValueToValidate)
				{
					for (auto& allowedValue : this->allowedValues)
					{
						if (boost::any_cast<std::string>(allowedValue) == actualValueToValidate)
							return Result();
					}
					return Result();
				}

				template Result BaseParameter::ValidateParameterValue<bool>(bool actualValueToValidate);
				template Result BaseParameter::ValidateParameterValue<std::uint16_t>(std::uint16_t actualValueToValidate);
				template Result BaseParameter::ValidateParameterValue<std::uint32_t>(std::uint32_t actualValueToValidate);
				template Result BaseParameter::ValidateParameterValue<std::uint64_t>(std::uint64_t actualValueToValidate);
				template Result BaseParameter::ValidateParameterValue<std::int16_t>(std::int16_t actualValueToValidate);
				template Result BaseParameter::ValidateParameterValue<std::int32_t>(std::int32_t actualValueToValidate);
				template Result BaseParameter::ValidateParameterValue<std::int64_t>(std::int64_t actualValueToValidate);

				template<typename T>
				T BaseParameter::GetTypedParameterActualValue()
				{
					if (this->GetDataType().is_initialized())
					{
						if (this->GetParameterActualValue().get().type() == typeid(T))
						{
							if (this->GetParameterActualValue().is_initialized())
								return boost::any_cast<T>(this->GetParameterActualValue().get());
							//No actual value present
							boost::format formatter(kMsgParameterActualValueDoesNotExist[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
							formatter
							% this->uniqueID;
							LOG_ERROR() << formatter.str();
							throw Result(ErrorCode::PARAMETER_HAS_NO_ACTUAL_VALUE, formatter.str());
						}
						//Datatype does not match
						boost::format formatter(kMsgBaseObjectDataTypeMismatch[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% this->GetUniqueID()
						% typeid(T).name()
						% this->GetParameterActualValue().get().type().name();
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::DATATYPE_MISMATCH, formatter.str());
					}
					boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
					formatter
					% this->GetUniqueID();
					LOG_ERROR() << formatter.str();
					throw Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
				}

				template<> std::string BaseParameter::GetTypedParameterActualValue<std::string>()
				{
					if (!this->GetDataType().is_initialized())
					{
						boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% this->GetUniqueID();
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
					}

					if (!this->GetParameterActualValue().is_initialized())
					{
						boost::format formatter(kMsgParameterActualValueDoesNotExist[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% this->uniqueID;
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::PARAMETER_HAS_NO_ACTUAL_VALUE, formatter.str());
					}

					std::stringstream convertString;
					switch (this->GetDataType().get())
					{
						case IEC_Datatype::BOOL:
							{
								bool res = boost::any_cast<bool>(this->GetParameterActualValue().get());
								return (res) ? "01" : "00";
							}
						case IEC_Datatype::SINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << boost::any_cast<std::int16_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::INT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << boost::any_cast<std::int16_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::DINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << boost::any_cast<std::int32_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::LINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << boost::any_cast<std::int64_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::BYTE:
						case IEC_Datatype::_CHAR:
						case IEC_Datatype::USINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << boost::any_cast<std::uint16_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::WORD:
						case IEC_Datatype::UINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << boost::any_cast<std::uint16_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::DWORD:
						case IEC_Datatype::UDINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << boost::any_cast<std::uint32_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::ULINT:
						case IEC_Datatype::LWORD:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << boost::any_cast<std::uint64_t>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::REAL:
							{
								convertString << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << FloatToSinglePrecisisionHex(boost::any_cast<float>(this->GetParameterActualValue().get()));
								return convertString.str();
							}
						case IEC_Datatype::LREAL:
							{
								convertString << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << DoubleToDoublePrecisisionHex(boost::any_cast<double>(this->GetParameterActualValue().get()));
								return convertString.str();
							}
						case IEC_Datatype::STRING:
						case IEC_Datatype::WSTRING:
							{
								convertString << boost::any_cast<std::string>(this->GetParameterActualValue().get());
								return convertString.str();
							}
						case IEC_Datatype::UNDEFINED:
						case IEC_Datatype::BITSTRING:
						default:
							break;
					}
					return "";
				}

				template bool BaseParameter::GetTypedParameterActualValue<bool>();
				template std::uint16_t BaseParameter::GetTypedParameterActualValue<std::uint16_t>();
				template std::uint32_t BaseParameter::GetTypedParameterActualValue<std::uint32_t>();
				template std::uint64_t BaseParameter::GetTypedParameterActualValue<std::uint64_t>();
				template std::int16_t BaseParameter::GetTypedParameterActualValue<std::int16_t>();
				template std::int32_t BaseParameter::GetTypedParameterActualValue<std::int32_t>();
				template std::int64_t BaseParameter::GetTypedParameterActualValue<std::int64_t>();
				template double BaseParameter::GetTypedParameterActualValue<double>();

				template<typename T>
				T BaseParameter::GetTypedParameterDefaultValue()
				{
					if (this->GetDataType().is_initialized())
					{
						if (this->GetParameterDefaultValue().get().type() == typeid(T))
						{
							if (this->GetParameterDefaultValue().is_initialized())
								return boost::any_cast<T>(this->GetParameterDefaultValue().get());
							//No default value present
							boost::format formatter(kMsgParameterDefaultValueDoesNotExist[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
							formatter
							% this->uniqueID;
							LOG_ERROR() << formatter.str();
							throw Result(ErrorCode::PARAMETER_HAS_NO_DEFAULT_VALUE, formatter.str());
						}
						//Datatype does not match
						boost::format formatter(kMsgBaseObjectDataTypeMismatch[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% this->GetUniqueID()
						% typeid(T).name()
						% this->GetParameterDefaultValue().get().type().name();
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::DATATYPE_MISMATCH, formatter.str());
					}
					boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
					formatter
					% this->GetUniqueID();
					LOG_ERROR() << formatter.str();
					throw Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
				}

				template<> std::string BaseParameter::GetTypedParameterDefaultValue<std::string>()
				{
					if (!this->GetDataType().is_initialized())
					{
						boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% this->GetUniqueID();
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
					}

					if (!this->GetParameterDefaultValue().is_initialized())
					{
						boost::format formatter(kMsgParameterDefaultValueDoesNotExist[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% this->uniqueID;
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::PARAMETER_HAS_NO_DEFAULT_VALUE, formatter.str());
					}

					std::stringstream convertString;
					switch (this->GetDataType().get())
					{
						case IEC_Datatype::BITSTRING:
							break;
						case IEC_Datatype::BOOL:
							{
								bool res = boost::any_cast<bool>(this->GetParameterDefaultValue().get());
								return (res) ? "01" : "00";
							}
						case IEC_Datatype::SINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << boost::any_cast<std::int16_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::INT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << boost::any_cast<std::int16_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::DINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << boost::any_cast<std::int32_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::LINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << boost::any_cast<std::int64_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::BYTE:
						case IEC_Datatype::_CHAR:
						case IEC_Datatype::USINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << boost::any_cast<std::uint16_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::WORD:
						case IEC_Datatype::UINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << boost::any_cast<std::uint16_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::DWORD:
						case IEC_Datatype::UDINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << boost::any_cast<std::uint32_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::ULINT:
							{
								convertString << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << boost::any_cast<std::uint64_t>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::LWORD:
						case IEC_Datatype::REAL:
							{
								convertString << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << FloatToSinglePrecisisionHex(boost::any_cast<float>(this->GetParameterDefaultValue().get()));
								return convertString.str();
							}
						case IEC_Datatype::LREAL:
							{
								convertString << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << DoubleToDoublePrecisisionHex(boost::any_cast<double>(this->GetParameterDefaultValue().get()));
								return convertString.str();
							}
						case IEC_Datatype::STRING:
						case IEC_Datatype::WSTRING:
							{
								convertString << boost::any_cast<std::string>(this->GetParameterDefaultValue().get());
								return convertString.str();
							}
						case IEC_Datatype::UNDEFINED:
						default:
							break;
					}
					return "";
				}

				template bool BaseParameter::GetTypedParameterDefaultValue<bool>();
				template std::uint16_t BaseParameter::GetTypedParameterDefaultValue<std::uint16_t>();
				template std::uint32_t BaseParameter::GetTypedParameterDefaultValue<std::uint32_t>();
				template std::uint64_t BaseParameter::GetTypedParameterDefaultValue<std::uint64_t>();
				template std::int16_t BaseParameter::GetTypedParameterDefaultValue<std::int16_t>();
				template std::int32_t BaseParameter::GetTypedParameterDefaultValue<std::int32_t>();
				template std::int64_t BaseParameter::GetTypedParameterDefaultValue<std::int64_t>();
				template float BaseParameter::GetTypedParameterDefaultValue<float>();
				template double BaseParameter::GetTypedParameterDefaultValue<double>();

				Result BaseParameter::AddParameterAllowedRange(const std::string& minValue, const std::string& maxValue)
				{
					try
					{
						//Set empty is valid behavior leave uninitialised
						if (maxValue.empty())
							return Result();

						if (!this->GetDataType().is_initialized())
						{
							boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
							formatter
							% this->GetUniqueID();
							LOG_FATAL() << formatter.str();
							return Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
						}

						switch (this->GetDataType().get())
						{
							case IEC_Datatype::BYTE:
							case IEC_Datatype::_CHAR:
							case IEC_Datatype::USINT:
								{
									std::uint16_t minValueNr = HexToInt<std::uint16_t>(minValue);
									std::uint16_t maxValueNr = HexToInt<std::uint16_t>(maxValue);
									if (minValueNr > 255 || maxValueNr > 255)
									{
										boost::format formatter(kMsgParameterMinMaxValueInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
										formatter
										% minValue
										% maxValue
										% IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
										return Result(ErrorCode::VALUE_NOT_WITHIN_RANGE, formatter.str());
									}
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::UINT:
							case IEC_Datatype::WORD:
								{
									std::uint16_t minValueNr = HexToInt<std::uint16_t>(minValue);
									std::uint16_t maxValueNr = HexToInt<std::uint16_t>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::UDINT:
							case IEC_Datatype::DWORD:
								{
									std::uint32_t minValueNr = HexToInt<std::uint32_t>(minValue);
									std::uint32_t maxValueNr = HexToInt<std::uint32_t>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::ULINT:
							case IEC_Datatype::LWORD:
								{
									std::uint64_t minValueNr = HexToInt<std::uint64_t>(minValue);
									std::uint64_t maxValueNr = HexToInt<std::uint64_t>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::SINT:
								{
									std::int16_t minValueNr = HexToInt<std::int16_t>(minValue);
									std::int16_t maxValueNr = HexToInt<std::int16_t>(maxValue);
									if (minValueNr < -128 || minValueNr > 127 || maxValueNr < -128 || maxValueNr > 127)
									{
										boost::format formatter(kMsgParameterMinMaxValueInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
										formatter
										% minValue
										% maxValue
										% IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
										return Result(ErrorCode::VALUE_NOT_WITHIN_RANGE, formatter.str());
									}
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::INT:
								{
									std::int16_t minValueNr = HexToInt<std::int16_t>(minValue);
									std::int16_t maxValueNr = HexToInt<std::int16_t>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::DINT:
								{
									std::int32_t minValueNr = HexToInt<std::int32_t>(minValue);
									std::int32_t maxValueNr = HexToInt<std::int32_t>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::LINT:
								{
									std::int64_t minValueNr = HexToInt<std::int64_t>(minValue);
									std::int64_t maxValueNr = HexToInt<std::int64_t>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::REAL:
								{
									float minValueNr = boost::lexical_cast<float>(minValue);
									float maxValueNr = boost::lexical_cast<float>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::LREAL:
								{
									double minValueNr = boost::lexical_cast<double>(minValue);
									double maxValueNr = boost::lexical_cast<double>(maxValue);
									this->allowedRanges.push_back(std::make_pair(minValueNr, maxValueNr));
									break;
								}
							case IEC_Datatype::STRING:
							case IEC_Datatype::WSTRING:
							case IEC_Datatype::UNDEFINED:
							case IEC_Datatype::BITSTRING:
							case IEC_Datatype::BOOL:
							default:
								{
									boost::format formatter(kMsgBaseObjectDoesNotSupportLimits[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
									formatter
									% IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
									LOG_FATAL() << formatter.str();
									return Result(ErrorCode::DATATYPE_DOES_NOT_SUPPORT_LIMITS, formatter.str());
								}
						}
					}
					catch (const std::exception&)
					{
						std::string dataTypeStr = "";
						if (this->GetDataType().is_initialized())
						{
							dataTypeStr = IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
						}
						else if (this->GetComplexDataType())
						{
							dataTypeStr = this->GetComplexDataType()->GetUniqueID();
						}
						boost::format formatter(kMsgParameterMinMaxValueInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% minValue
						% maxValue
						% dataTypeStr;
						return Result(ErrorCode::VALUE_NOT_WITHIN_RANGE, formatter.str());
					}
					return Result();
				}

				Result BaseParameter::AddParameterValue(const std::string& valueToSet, ValueType type)
				{
					try
					{
						//Set empty is valid behavior leave uninitialised
						if (valueToSet.empty())
						{
							this->ClearActualValue();
							return Result();
						}

						if (!this->GetDataType().is_initialized())
						{
							// Object has not datatype defined
							boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
							formatter
							% this->GetUniqueID();
							LOG_FATAL() << formatter.str();
							return Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
						}
						boost::any any;

						switch (this->GetDataType().get())
						{
							case IEC_Datatype::BYTE:
							case IEC_Datatype::_CHAR:
							case IEC_Datatype::USINT:
								{
									std::uint16_t value = HexToInt<std::uint16_t>(valueToSet);
									if (value > 255)
									{
										boost::format formatter(kMsgParameterValueInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
										formatter
										% valueToSet
										% IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
										return Result(ErrorCode::PARAMETER_VALUE_INVALID, formatter.str());
									}
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::uint16_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::uint16_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::UINT:
							case IEC_Datatype::WORD:
								{
									std::uint16_t value = HexToInt<std::uint16_t>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::uint16_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::uint16_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::UDINT:
							case IEC_Datatype::DWORD:
								{
									std::uint32_t value = HexToInt<std::uint32_t>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::uint32_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::uint32_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::ULINT:
							case IEC_Datatype::LWORD:
								{
									std::uint64_t value = HexToInt<std::uint64_t>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::uint64_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::uint64_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::SINT:
								{
									std::int16_t value = HexToInt<std::int16_t>(valueToSet);
									if (value < -128 || value > 127)
										throw std::range_error("");
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::int16_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::int16_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::INT:
								{
									std::int16_t value = HexToInt<std::int16_t>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::int16_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::int16_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::DINT:
								{
									std::int32_t value = HexToInt<std::int32_t>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::int32_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::int32_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::LINT:
								{
									std::int64_t value = HexToInt<std::int64_t>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::int64_t>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::int64_t>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::REAL:
								{
									float value = boost::lexical_cast<float>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<float>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (std::fabs(this->GetTypedParameterDefaultValue<float>() - value) > std::numeric_limits<float>::epsilon())
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::LREAL:
								{
									double value = boost::lexical_cast<double>(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<double>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (std::fabs(this->GetTypedParameterDefaultValue<double>() - value) > std::numeric_limits<double>::epsilon())
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::BOOL:
								{
									bool value = StringToBool(valueToSet);
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<bool>(value);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<bool>() != value)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(value);
									break;
								}
							case IEC_Datatype::STRING:
							case IEC_Datatype::WSTRING:
								{
									if (type == ValueType::DEFAULT || type == ValueType::ACTUAL)
									{
										Result res = ValidateParameterValue<std::string>(valueToSet);
										if (!res.IsSuccessful())
											return res;
									}
									if (type == ValueType::ACTUAL)
									{
										if (!this->GetParameterDefaultValue().is_initialized())
											this->actualValueNotDefaultValue = true;
										else if (this->GetTypedParameterDefaultValue<std::string>() != valueToSet)
											this->actualValueNotDefaultValue = true;
										else
											this->actualValueNotDefaultValue = false;
									}
									any = boost::any(valueToSet);
									break;
								}
							case IEC_Datatype::UNDEFINED:
							case IEC_Datatype::BITSTRING:
							default:
								{
									boost::format formatter(kMsgParameterValueInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
									formatter
									% valueToSet
									% IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
									return Result(ErrorCode::PARAMETER_VALUE_INVALID, formatter.str());
								}
						}

						switch (type)
						{
							case ValueType::ACTUAL:
								{
									this->actualValue = any;
									break;
								}
							case ValueType::DEFAULT:
								{
									this->defaultValue = any;
									break;
								}
							case ValueType::ALLOWED:
								{
									this->allowedValues.push_back(any);
									break;
								}
							default:
								break;
						}
					}
					catch (const std::exception&)
					{
						std::string dataTypeStr = "";
						if (this->GetDataType().is_initialized())
						{
							dataTypeStr = IECDatatypeValues[static_cast<std::underlying_type<IEC_Datatype>::type>(this->GetDataType().get())];
						}
						else if (this->GetComplexDataType())
						{
							dataTypeStr = this->GetComplexDataType()->GetUniqueID();
						}
						boost::format formatter(kMsgParameterValueInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
						% valueToSet
						% dataTypeStr;
						return Result(ErrorCode::PARAMETER_VALUE_INVALID, formatter.str());
					}
					return Result();
				}

				Result BaseParameter::AddParameterAllowedValue(const std::string& allowedValue)
				{
					return AddParameterValue(allowedValue, ValueType::ALLOWED);
				}

				Result BaseParameter::AddParameterDefaultValue(const std::string& defaultValueToSet)
				{
					return AddParameterValue(defaultValueToSet, ValueType::DEFAULT);
				}

				Result BaseParameter::SetParameterActualValue(const std::string& actualValueToSet)
				{
					return AddParameterValue(actualValueToSet, ValueType::ACTUAL);
				}

				bool BaseParameter::WriteToConfiguration() const
				{
					if ((this->actualValueNotDefaultValue == true) //Actual value != default value
					        && (this->GetParameterAccess() != ParameterAccess::constant //correct AccessType
					            && this->GetParameterAccess() != ParameterAccess::noAccess
					            && this->GetParameterAccess() != ParameterAccess::read
					            && this->GetParameterAccess() != ParameterAccess::undefined))
						return true;
					else
						return false;
				}

				const boost::dynamic_bitset<>& BaseParameter::GetParameterActualValueBitSet(const std::string& tempActualValue)
				{
					std::string currentActualValue = "";
					bool clearActualValue = false;
					if (!this->HasActualValue() && tempActualValue.empty())
					{
						if (this->HasDefaultValue())
						{
							this->actualValue = this->defaultValue;
							clearActualValue = true;
						}
					}

					if (!tempActualValue.empty())
					{
						if (this->HasActualValue())
							currentActualValue = this->GetTypedParameterActualValue<std::string>();
						this->SetParameterActualValue(tempActualValue);
					}

					if (!this->GetDataType().is_initialized())
					{
						boost::format formatter(kMsgParameterDataTypeInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
							% this->GetUniqueID();
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::PARAMETER_HAS_NO_DATATYPE, formatter.str());
					}

					if (!this->GetParameterActualValue().is_initialized())
					{
						boost::format formatter(kMsgParameterActualValueDoesNotExist[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
						formatter
							% this->uniqueID;
						LOG_ERROR() << formatter.str();
						throw Result(ErrorCode::PARAMETER_HAS_NO_ACTUAL_VALUE, formatter.str());
					}

					this->actualValueBitset.clear();
					switch (this->GetDataType().get())
					{
						case IEC_Datatype::BOOL:
							{
								bool res = boost::any_cast<bool>(this->GetParameterActualValue().get());
								this->actualValueBitset.push_back(res);
								this->actualValueBitset.resize(1);
								break;
							}
						case IEC_Datatype::SINT:
							{
								std::bitset<8> actualValueSet(boost::any_cast<std::int16_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(8);
								break;
							}
						case IEC_Datatype::INT:
							{
								std::bitset<16> actualValueSet(boost::any_cast<std::int16_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(16);
								break;
							}
						case IEC_Datatype::DINT:
							{
								std::bitset<32> actualValueSet(boost::any_cast<std::int32_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(32);
								break;
							}
						case IEC_Datatype::LINT:
							{
								std::bitset<64> actualValueSet(boost::any_cast<std::int64_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(64);
								break;
							}
						case IEC_Datatype::BYTE:
						case IEC_Datatype::_CHAR:
						case IEC_Datatype::USINT:
							{
								std::bitset<8> actualValueSet(boost::any_cast<std::uint16_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(8);
								break;
							}
						case IEC_Datatype::WORD:
						case IEC_Datatype::UINT:
							{
								std::bitset<16> actualValueSet(boost::any_cast<std::uint16_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(16);
								break;
							}
						case IEC_Datatype::DWORD:
						case IEC_Datatype::UDINT:
							{
								std::bitset<32> actualValueSet(boost::any_cast<std::uint32_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(32);
								break;
							}
						case IEC_Datatype::ULINT:
						case IEC_Datatype::LWORD:
							{
								std::bitset<64> actualValueSet(boost::any_cast<std::uint64_t>(this->GetParameterActualValue().get()));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(64);
								break;
							}
						case IEC_Datatype::REAL:
							{
								std::bitset<32> actualValueSet(FloatToSinglePrecisisionHex(boost::any_cast<float>(this->GetParameterActualValue().get())));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(32);
								break;
							}
						case IEC_Datatype::LREAL:
							{
								std::bitset<64> actualValueSet(DoubleToDoublePrecisisionHex(boost::any_cast<double>(this->GetParameterActualValue().get())));
								this->actualValueBitset.append(actualValueSet.to_ulong());
								this->actualValueBitset.resize(64);
								break;
							}
						case IEC_Datatype::STRING:
						case IEC_Datatype::WSTRING:
							{
								std::string actVal = boost::any_cast<std::string>(this->GetParameterActualValue().get());
								for (std::uint32_t i = 0; i < actVal.length(); ++i)
								{
									this->actualValueBitset.append(unsigned(actVal.at(i)));
								}
								break;
							}
						case IEC_Datatype::BITSTRING:
						case IEC_Datatype::UNDEFINED:
						default:
							break;
					}

					if (!tempActualValue.empty() && !currentActualValue.empty())
					{
						this->SetParameterActualValue(currentActualValue);
					}
					else if (!tempActualValue.empty() || clearActualValue)
						this->ClearActualValue();

					return this->actualValueBitset;
				}

				void BaseParameter::ClearActualValue()
				{
					this->actualValue.reset();
					this->actualValueNotDefaultValue = false;
				}
			}
		}
	}
}
