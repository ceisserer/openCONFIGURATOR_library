/************************************************************************
\file OpenConfiguratorCore.cpp
\brief Implementation of the Class OpenConfiguratorCore
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
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES{}
LOSS OF USE, DATA, OR PROFITS{} OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/
#include "OpenConfiguratorCore.h"

using namespace IndustrialNetwork::POWERLINK::Core::API;
using namespace IndustrialNetwork::POWERLINK::Core::ErrorHandling;
using namespace IndustrialNetwork::POWERLINK::Core::Configuration;
using namespace IndustrialNetwork::POWERLINK::Core::NetworkHandling;
using namespace IndustrialNetwork::POWERLINK::Core::Node;
using namespace IndustrialNetwork::POWERLINK::Core::ObjectDictionary;
using namespace IndustrialNetwork::POWERLINK::Core::CoreConfiguration;
using namespace IndustrialNetwork::POWERLINK::Core::Utilities;
using namespace IndustrialNetwork::POWERLINK::Core::ModularNode;

OpenConfiguratorCore::OpenConfiguratorCore()
{}

OpenConfiguratorCore::~OpenConfiguratorCore()
{}

OpenConfiguratorCore& OpenConfiguratorCore::GetInstance()
{
	static OpenConfiguratorCore instance;
	return instance;
}

Result OpenConfiguratorCore::InitLoggingConfiguration(const std::string& configuration)
{
	return ProjectManager::GetInstance().InitLoggingConfiguration(configuration);
}

Result OpenConfiguratorCore::InitEclipseLoggingPath(const std::string& loggingPath)
{
	return ProjectManager::GetInstance().InitEclipseLoggingConfiguration(loggingPath);
}

Result OpenConfiguratorCore::SetLoggingLanguage(const Language& lang)
{
	LoggingConfiguration::GetInstance().SetCurrentLanguage(lang);
	return Result();
}

Result OpenConfiguratorCore::GetSupportedSettingIds(std::vector<std::string>& supportedSettings)
{
	supportedSettings = ProjectManager::GetInstance().GetSupportedSettingIds();
	return Result();
}

Result OpenConfiguratorCore::CreateNetwork(const std::string& networkId)
{
	std::shared_ptr<Network> net = std::make_shared<Network>(networkId);
	return ProjectManager::GetInstance().AddNetwork(networkId, net);
}

Result OpenConfiguratorCore::RemoveNetwork(const std::string& networkId)
{
	return ProjectManager::GetInstance().RemoveNetwork(networkId);
}

Result OpenConfiguratorCore::GetNetworkIds(std::vector<std::string>& networkList)
{
	networkList = ProjectManager::GetInstance().GetNetworkIds();
	return Result();
}

Result OpenConfiguratorCore::ClearNetworks()
{
	return ProjectManager::GetInstance().ClearNetworkList();
}

Result OpenConfiguratorCore::GetNetwork(const std::string& networkId, Network& network)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (res.IsSuccessful())
			network = *networkPtr.get();
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetCycleTime(const std::string& networkId, std::uint32_t cycleTime)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = networkPtr->SetCycleTime(cycleTime);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> obj;
		res = nodePtr->GetObject(0x1006, obj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (obj->GetForceToCDC())
		{
			std::stringstream str;
			str << cycleTime;
			boost::format formatter(kMsgForcedValueOverwriteObject[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% str.str()
			% obj->GetObjectId()
			% (std::uint32_t) 240;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::FORCED_VALUE_OVERWRITE, "[" + networkId + "] " + formatter.str());
		}

		res = nodePtr->ForceObject(0x1006, false, false, IntToHex(cycleTime, 8, "0x"));
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetAsyncMtu(const std::string& networkId, std::uint16_t asyncMtu)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		networkPtr->SetAsyncMTU(asyncMtu);

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subObj;
		res = nodePtr->GetSubObject(0x1F98, 0x8, subObj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = nodePtr->ForceSubObject(0x1f98, 0x8, subObj->GetForceToCDC(), false, IntToHex(asyncMtu, 4, "0x"));
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetMultiplexedCycleCount(const std::string& networkId, std::uint16_t multiplexedCycleCount)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		networkPtr->SetMultiplexedCycleCount(multiplexedCycleCount);

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subObj;
		res = nodePtr->GetSubObject(0x1F98, 0x7, subObj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = nodePtr->ForceSubObject(0x1f98, 0x7, subObj->GetForceToCDC(), false, IntToHex(multiplexedCycleCount, 2, "0x"));
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetPrescaler(const std::string& networkId, std::uint16_t prescaler)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		networkPtr->SetPrescaler(prescaler);

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subObj;
		res = nodePtr->GetSubObject(0x1F98, 0x9, subObj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = nodePtr->ForceSubObject(0x1f98, 0x9, subObj->GetForceToCDC(), false, IntToHex(prescaler, 2, "0x"));
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::BuildConfiguration(const std::string& networkId, std::string& configurationOutput, std::vector<std::uint8_t>& binOutput)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		std::stringstream config;
		std::stringstream hexOutput;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!networkPtr->HasControlledNodes())
			return Result(ErrorCode::NO_CONTROLLED_NODES_CONFIGURED, "[" + networkId + "] " + kMsgNoNodesConfigured[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);

		res = networkPtr->GenerateConfiguration();
		if (res.IsSuccessful())
			res = ConfigurationGenerator::GetInstance().GenerateNetworkConfiguration(networkPtr, config, hexOutput);

		configurationOutput = config.str();
		ConfigurationToAscii(hexOutput, binOutput);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::BuildXMLProcessImage(const std::string& networkId, const std::uint8_t nodeid, std::string& processImageOutput)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!networkPtr->HasControlledNodes())
			return Result(ErrorCode::NO_CONTROLLED_NODES_CONFIGURED, "[" + networkId + "] " + kMsgNoNodesConfigured[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);

		res = networkPtr->GenerateConfiguration();
		if (res.IsSuccessful())
			processImageOutput = XmlProcessImageGenerator::GetInstance().Generate(nodeid, networkPtr);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::BuildNETProcessImage(const std::string& networkId, const std::uint8_t nodeid, std::string& processImageOutput)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!networkPtr->HasControlledNodes())
			return Result(ErrorCode::NO_CONTROLLED_NODES_CONFIGURED, "[" + networkId + "] " + kMsgNoNodesConfigured[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);

		res = networkPtr->GenerateConfiguration();
		if (res.IsSuccessful())
			processImageOutput = NetProcessImageGenerator::GetInstance().Generate(nodeid, networkPtr);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::BuildCProcessImage(const std::string& networkId, const std::uint8_t nodeid, std::string& processImageOutput)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!networkPtr->HasControlledNodes())
			return Result(ErrorCode::NO_CONTROLLED_NODES_CONFIGURED, "[" + networkId + "] " + kMsgNoNodesConfigured[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);

		res = networkPtr->GenerateConfiguration();
		if (res.IsSuccessful())
			processImageOutput = CProcessImageGenerator::GetInstance().Generate(nodeid, networkPtr);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateNode(const std::string& networkId, const std::uint8_t nodeID, const std::string& nodeName, const bool isRmn)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
		{
			//Add active managing node to network
			if (nodeID == 240)
			{
				std::shared_ptr<ManagingNode> node = std::make_shared<ManagingNode>(nodeID, nodeName);
				res = network->AddNode(node);
			}
			//Add redundant managing node to network
			else if ((nodeID >= 241 && nodeID <= 250) || isRmn) //allow the RMN to have normal nodeIDs
			{
				std::shared_ptr<ManagingNode> node = std::make_shared<ManagingNode>(nodeID, nodeName);
				res = network->AddNode(node);
			}
			//Add normal controlled node to network
			else if (nodeID >= 1 && nodeID <= 239)
			{
				std::shared_ptr<ControlledNode> node = std::make_shared<ControlledNode>(nodeID, nodeName);
				res = network->AddNode(node);
			}
			else
			{
				//Nodeid invalid
				boost::format formatter(kMsgNodeIdInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
				formatter
				% (std::uint32_t) nodeID;
				LOG_ERROR() << formatter.str();
				return Result(ErrorCode::NODEID_INVALID, "[" + networkId + "] " + formatter.str());
			}
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::RemoveNode(const std::string& networkId, const std::uint8_t nodeID)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->RemoveNode(nodeID);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetControlledNode(const std::string& networkId, const std::uint8_t nodeID, ControlledNode& node)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
		{
			std::shared_ptr<BaseNode> nodePtr;
			res = network->GetBaseNode(nodeID, nodePtr);
			if (res.IsSuccessful())
			{
				std::shared_ptr<ControlledNode> cn = std::dynamic_pointer_cast<ControlledNode>(nodePtr);
				if (cn)
					node = *cn.get();
				else
				{
					boost::format formatter(kMsgNonControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
					formatter
					% (std::uint32_t) nodeID;
					LOG_ERROR() << "[" + networkId + "] " + formatter.str();
					return Result(ErrorCode::NODE_IS_NOT_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
				}
			}
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetManagingNode(const std::string& networkId, ManagingNode& node)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
		{
			std::shared_ptr<ManagingNode> nodePtr;
			res = network->GetManagingNode(nodePtr);
			if (res.IsSuccessful())
			{
				node = *nodePtr.get();
			}
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetAvailableNodeIds(const std::string& networkId, std::vector<std::uint8_t>& nodeIdCollection)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->GetAvailableNodeIds(nodeIdCollection);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateConfiguration(const std::string& networkId, const std::string& configID)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->AddConfiguration(configID);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::RemoveConfiguration(const std::string& networkId, const std::string& configID)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->RemoveConfiguration(configID);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::ReplaceConfigurationName(const std::string& networkId, const std::string& oldConfigId, const std::string& newConfigId)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->ReplaceConfigurationName(oldConfigId, newConfigId);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateConfigurationSetting(const std::string& networkId, const std::string& configID, const std::string& name, const std::string& value)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->AddConfigurationSetting(configID, std::make_shared<BuildConfigurationSetting>(name, value));

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::RemoveConfigurationSetting(const std::string& networkId, const std::string& configID, const std::string& name)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->RemoveConfigurationSetting(configID, name);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetConfigurationSettingEnabled(const std::string& networkId, const std::string& configID, const std::string& settingID, bool enabled)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->SetConfigurationSettingEnabled(configID, settingID, enabled);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetConfigurationSettings(const std::string& networkId, const std::string& configID, std::vector<BuildConfigurationSetting>& settings)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
		{
			std::vector<std::shared_ptr<BuildConfigurationSetting>> configurationSettings;
			res = network->GetConfigurationSettings(configID, configurationSettings);
			if (res.IsSuccessful())
			{
				for (auto& config : configurationSettings)
				{
					settings.push_back(*config.get());
				}
			}
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetActiveConfiguration(const std::string& networkId, std::string& activeConfiguration)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			activeConfiguration = network->GetActiveConfiguration();

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetActiveConfiguration(const std::string& networkId, const std::string& configID)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->SetActiveConfiguration(configID);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetBuildConfigurations(const std::string& networkId, std::vector<PlkConfiguration>& buildConfigurations)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::vector<std::shared_ptr<PlkConfiguration>> configurations;
		res = network->GetBuildConfigurations(configurations);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		for (auto& config : configurations)
		{
			buildConfigurations.push_back(*config.get());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> ptr(new Object(objectId, objectType, name, nodeId));

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = node->AddObject(ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = node->ForceObject(objectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetObjectLimits(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, const std::string& lowLimit, const std::string& highLimit)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> obj;
		res = node->GetObject(objectId, obj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = obj->SetLowLimit(lowLimit);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = obj->SetHighLimit(highLimit);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateDomainObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, ObjectType objectType, const std::string& name, PDOMapping pdoMapping, const std::string& uniqueIdRef)
{
	try
	{
		Result res = OpenConfiguratorCore::CreateParameterObject(networkId, nodeId, objectId, objectType, name, PlkDataType::Domain, AccessType::UNDEFINED, pdoMapping, uniqueIdRef, "", "");
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameterObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> ptr(new Object(objectId, objectType, name, nodeId));

		std::shared_ptr<Parameter> param;
		res = node->GetApplicationProcess()->GetParameter(uniqueIdRef, param);
		if (!res.IsSuccessful())
		{
			std::shared_ptr<ParameterGroup> paramGrp;
			res = node->GetApplicationProcess()->GetParameterGroup(uniqueIdRef, paramGrp);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			else
				ptr->SetComplexDataType(paramGrp);
		}
		else
		{
			ptr->SetComplexDataType(param);
		}

		ptr->SetUniqueIdRef(uniqueIdRef);

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);
		else if (dataType == PlkDataType::UNDEFINED && !uniqueIdRef.empty())
			ptr->SetDataType(PlkDataType::Domain);
		else
		{
			boost::format formatter(kMsgBaseObjectDataTypeError[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% name
			% objectId
			% (std::uint32_t) nodeId;
			LOG_FATAL() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_DATATYPE, formatter.str());
		}

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = node->AddObject(ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = node->ForceObject(objectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateSubObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> ptr(new SubObject(subObjectId, objectType, name, nodeId));

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = node->AddSubObject(objectId, ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = node->ForceSubObject(objectId, subObjectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetSubObjectLimits(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, const std::string& lowLimit, const std::string& highLimit)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subObj;
		res = node->GetSubObject(objectId, subObjectId, subObj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = subObj->SetLowLimit(lowLimit);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = subObj->SetHighLimit(highLimit);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateDomainSubObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, ObjectType objectType, const std::string& name, PDOMapping pdoMapping, const std::string& uniqueIdRef)
{
	try
	{
		Result res = OpenConfiguratorCore::CreateParameterSubObject(networkId, nodeId, objectId, subObjectId, objectType, name, PlkDataType::Domain, AccessType::UNDEFINED, pdoMapping, uniqueIdRef, "", "");
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameterSubObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> ptr(new SubObject(subObjectId, objectType, name, nodeId));

		std::shared_ptr<Parameter> param;
		res = node->GetApplicationProcess()->GetParameter(uniqueIdRef, param);
		if (!res.IsSuccessful())
		{
			std::shared_ptr<ParameterGroup> paramGrp;
			res = node->GetApplicationProcess()->GetParameterGroup(uniqueIdRef, paramGrp);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			else
				ptr->SetComplexDataType(paramGrp);
		}
		else
		{
			ptr->SetComplexDataType(param);
		}

		ptr->SetUniqueIdRef(uniqueIdRef);

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);
		else if (dataType == PlkDataType::UNDEFINED && !uniqueIdRef.empty())
			ptr->SetDataType(PlkDataType::Domain);
		else
		{
			boost::format formatter(kMsgBaseObjectDataTypeError[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% name
			% objectId
			% (std::uint32_t) nodeId;
			LOG_FATAL() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_DATATYPE, formatter.str());
		}

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = node->AddSubObject(objectId, ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = node->ForceSubObject(objectId, subObjectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetObjectSize(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint32_t& size)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> object;
		res = node->GetObject(objectId, object);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		size = object->GetBitSize() / 8;
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetSubObjectSize(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, uint32_t& size)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subObject;
		res = node->GetSubObject(objectId, subObjectId, subObject);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		size = subObject->GetBitSize() / 8;
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetFeatureDefaultValue(CNFeatureEnum feature, std::string& value)
{
	value = PlkFeatureDefaultValues[feature];
	return Result();
}
Result OpenConfiguratorCore::GetFeatureDefaultValue(MNFeatureEnum feature, std::string& value)
{
	value = PlkFeatureDefaultValues[feature];
	return Result();
}
Result OpenConfiguratorCore::GetFeatureDefaultValue(GeneralFeatureEnum feature, std::string& value)
{
	value = PlkFeatureDefaultValues[feature];
	return Result();
}

Result OpenConfiguratorCore::SetFeatureValue(const std::string& networkId, const std::uint8_t nodeId, CNFeatureEnum feature, const std::string& value)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->GetNetworkManagement()->SetFeatureUntypedActualValue(feature, value);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetFeatureValue(const std::string& networkId, const std::uint8_t nodeId, MNFeatureEnum feature, const std::string& value)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->GetNetworkManagement()->SetFeatureUntypedActualValue(feature, value);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetFeatureValue(const std::string& networkId, const std::uint8_t nodeId, GeneralFeatureEnum feature, const std::string& value)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->GetNetworkManagement()->SetFeatureUntypedActualValue(feature, value);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameter(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, ParameterAccess access, IEC_Datatype dataType, bool createTemplate, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->CreateParamMapping(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		if (createTemplate)
		{
			auto ptr = std::make_shared<ParameterTemplate>(mappedParamUniqueId, access, dataType);
			res = node->GetApplicationProcess()->AddParameterTemplate(ptr);
		}
		else
		{
			auto ptr = std::make_shared<Parameter>(mappedParamUniqueId, access, dataType);
			res = node->GetApplicationProcess()->AddParameter(ptr);
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameter(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, ParameterAccess access, const std::string& parameterTemplateUniqueIdRef, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::string mappedparameterTemplateUniqueIdRef = parameterTemplateUniqueIdRef;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->GetMappedParameterName(parameterTemplateUniqueIdRef, mappedparameterTemplateUniqueIdRef);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->CreateParamMapping(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ParameterTemplate> templ;
		res = node->GetApplicationProcess()->GetParameterTemplate(mappedparameterTemplateUniqueIdRef, templ);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::make_shared<Parameter>(mappedParamUniqueId, parameterTemplateUniqueIdRef);
		ptr->SetParameterTemplate(templ);

		if (access != ParameterAccess::undefined)
			ptr->SetParameterAccess(access);

		res = node->GetApplicationProcess()->AddParameter(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameter(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueID, const std::string& datatypeUniqueIDRef, ParameterAccess access, bool createTemplate, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueID;
		std::string mappedDatatypeUniqueIDRef = datatypeUniqueIDRef;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueID + "_" + boost::uuids::to_string(uuid);

			res = module->GetMappedParameterName(datatypeUniqueIDRef, mappedDatatypeUniqueIDRef);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->CreateParamMapping(uniqueID, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		if (createTemplate)
		{
			auto ptr = std::make_shared<ParameterTemplate>(mappedParamUniqueId, access, mappedDatatypeUniqueIDRef);
			std::shared_ptr<ComplexDataType> dt;
			res = node->GetApplicationProcess()->GetComplexDataType(mappedDatatypeUniqueIDRef, dt);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			ptr->SetComplexDataType(dt);
			res = node->GetApplicationProcess()->AddParameterTemplate(ptr);
		}
		else
		{
			auto ptr = std::make_shared<Parameter>(mappedParamUniqueId, access, mappedDatatypeUniqueIDRef);
			std::shared_ptr<ComplexDataType> dt;
			res = node->GetApplicationProcess()->GetComplexDataType(mappedDatatypeUniqueIDRef, dt);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			ptr->SetComplexDataType(dt);
			res = node->GetApplicationProcess()->AddParameter(ptr);
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateStructDatatype(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, const std::string& name, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->CreateParamMapping(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		auto ptr = std::shared_ptr<ComplexDataType>(new StructDataType(mappedParamUniqueId, name));
		res = node->GetApplicationProcess()->AddComplexDataType(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateVarDeclaration(const std::string& networkId, const std::uint8_t nodeId, const std::string& structUniqueId, const std::string& uniqueId, const std::string& name, IEC_Datatype datatype, std::uint32_t size, const std::string& initialValue, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::string mappedStructUniqueId = structUniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		//if BITSTRING > 8 is added split into more VarDeclarations
		if (datatype == IEC_Datatype::BITSTRING && size > 8)
		{
			std::uint32_t count = 0;
			while (size != 0)
			{
				std::string uniqueIdNew = uniqueId;
				if (count != 0)
					uniqueIdNew = uniqueId + "_" + Utilities::IntToHex<std::uint32_t>(count, 2);

				if (size >= 64)
				{
					res = this->CreateVarDeclaration(networkId, nodeId, structUniqueId, uniqueIdNew, uniqueIdNew, IEC_Datatype::ULINT, 1, initialValue, interfaceId, moduleId, modulePosition);
					if (!res.IsSuccessful())
						return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
					size -= 64;
				}
				else if (size >= 32 && size < 64)
				{
					res = this->CreateVarDeclaration(networkId, nodeId, structUniqueId, uniqueIdNew, uniqueIdNew, IEC_Datatype::UDINT, 1, initialValue, interfaceId, moduleId, modulePosition);
					if (!res.IsSuccessful())
						return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
					size -= 32;
				}
				else if (size >= 16 && size < 32)
				{
					res = this->CreateVarDeclaration(networkId, nodeId, structUniqueId, uniqueIdNew, uniqueIdNew, IEC_Datatype::UINT, 1, initialValue, interfaceId, moduleId, modulePosition);
					if (!res.IsSuccessful())
						return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
					size -= 16;
				}
				else if (size >= 8 && size < 16)
				{
					res = this->CreateVarDeclaration(networkId, nodeId, structUniqueId, uniqueIdNew, uniqueIdNew, IEC_Datatype::USINT, 1, initialValue, interfaceId, moduleId, modulePosition);
					if (!res.IsSuccessful())
						return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
					size -= 8;
				}
				else if (size < 8)
				{
					res = this->CreateVarDeclaration(networkId, nodeId, structUniqueId, uniqueIdNew, uniqueIdNew, IEC_Datatype::BITSTRING, size, initialValue, interfaceId, moduleId, modulePosition);
					if (!res.IsSuccessful())
						return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

					uniqueIdNew += "_UnusedData";
					res = this->CreateVarDeclaration(networkId, nodeId, structUniqueId, uniqueIdNew, uniqueIdNew, IEC_Datatype::BITSTRING, 8 - size, initialValue, interfaceId, moduleId, modulePosition);
					if (!res.IsSuccessful())
						return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
					size -= size;
				}
				count++;
			}
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->GetMappedParameterName(structUniqueId, mappedStructUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->CreateParamMapping(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ComplexDataType> dt;
		res = node->GetApplicationProcess()->GetComplexDataType(mappedStructUniqueId, dt);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<StructDataType>(dt);
		auto sharedVar = std::make_shared<VarDeclaration>(mappedParamUniqueId, name, datatype, size, initialValue);
		res = ptr->AddVarDeclaration(sharedVar);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateArrayDatatype(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, const std::string& name, std::uint32_t lowerLimit, std::uint32_t upperLimit, IEC_Datatype dataType, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->CreateParamMapping(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		auto ptr = std::shared_ptr<ComplexDataType>(new ArrayDataType(mappedParamUniqueId, name, lowerLimit, upperLimit, dataType));
		res = node->GetApplicationProcess()->AddComplexDataType(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateEnumDatatype(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, const std::string& name, IEC_Datatype dataType, std::uint32_t size, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = uniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->CreateParamMapping(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		auto ptr = std::shared_ptr<ComplexDataType>(new EnumDataType(mappedParamUniqueId, name, dataType, size));
		res = node->GetApplicationProcess()->AddComplexDataType(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateEnumValue(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, const std::string& name, const std::string& value, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = uniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->GetMappedParameterName(uniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			std::shared_ptr<ComplexDataType> dt;
			res = module->GetApplicationProcess()->GetComplexDataType(uniqueId, dt);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			auto ptr = std::dynamic_pointer_cast<EnumDataType>(dt);
			res = ptr->AddEnumValue(name, value);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ComplexDataType> dt;
		res = node->GetApplicationProcess()->GetComplexDataType(mappedParamUniqueId, dt);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<EnumDataType>(dt);
		res = ptr->AddEnumValue(name, value);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetParameterAllowedValues(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, std::vector<std::string>& allowedValues)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Parameter> param;
		res = node->GetApplicationProcess()->GetParameter(parameterUniqueId, param);
		if (res.IsSuccessful())
		{
			for (auto& allowedValue : allowedValues)
			{
				res = param->AddParameterAllowedValue(allowedValue);
				if (!res.IsSuccessful())
					return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			}
			return Result();
		}

		std::shared_ptr<ParameterTemplate> templ;
		res = node->GetApplicationProcess()->GetParameterTemplate(parameterUniqueId, templ);
		if (res.IsSuccessful())
		{
			for (auto& allowedValue : allowedValues)
			{
				res = templ->AddParameterAllowedValue(allowedValue);
				if (!res.IsSuccessful())
					return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			}
		}
		return Result();
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetParameterDefaultValue(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& paramDefaultValue)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Parameter> param;
		res = node->GetApplicationProcess()->GetParameter(parameterUniqueId, param);
		if (res.IsSuccessful())
			return param->AddParameterDefaultValue(paramDefaultValue);

		std::shared_ptr<ParameterTemplate> templ;
		res = node->GetApplicationProcess()->GetParameterTemplate(parameterUniqueId, templ);
		if (res.IsSuccessful())
			return templ->AddParameterDefaultValue(paramDefaultValue);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetParameterActualValue(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& paramActualValue)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->GetApplicationProcess()->SetParameterActualValue(parameterUniqueId, paramActualValue);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetParameterAllowedRange(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& minValue, const std::string& maxValue)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Parameter> param;
		res = node->GetApplicationProcess()->GetParameter(parameterUniqueId, param);
		if (res.IsSuccessful())
			return param->AddParameterAllowedRange(minValue, maxValue);

		std::shared_ptr<ParameterTemplate> templ;
		res = node->GetApplicationProcess()->GetParameterTemplate(parameterUniqueId, templ);
		if (res.IsSuccessful())
			return templ->AddParameterAllowedRange(minValue, maxValue);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameterGroup(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParamUniqueId = parameterGroupUniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParamUniqueId = parameterGroupUniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->CreateParamMapping(parameterGroupUniqueId, mappedParamUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		auto ptr = std::make_shared<ParameterGroup>(mappedParamUniqueId);
		res = node->GetApplicationProcess()->AddParameterGroup(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameterGroup(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& parentParameterGroupUniqueId, std::uint16_t bitOffset, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParameterGroupUniqueId = parameterGroupUniqueId;
		std::string mappedparentParameterGroupUniqueId = parentParameterGroupUniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParameterGroupUniqueId = parameterGroupUniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->GetMappedParameterName(parentParameterGroupUniqueId, mappedparentParameterGroupUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->CreateParamMapping(parameterGroupUniqueId, mappedParameterGroupUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ParameterGroup> ptr(new ParameterGroup(mappedParameterGroupUniqueId, bitOffset));
		std::shared_ptr<ParameterGroup> grp;
		res = node->GetApplicationProcess()->GetParameterGroup(mappedparentParameterGroupUniqueId, grp);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = grp->AddParameterGroupEntry(ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->GetApplicationProcess()->AddParameterGroup(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameterGroup(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& parentParameterGroupUniqueId, const std::string& conditionalUniqueId, const std::string& conditionalValue, std::uint16_t bitOffset, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParameterGroupUniqueId = parameterGroupUniqueId;
		std::string mappedparentParameterGroupUniqueId = parentParameterGroupUniqueId;
		std::string mappedconditionalUniqueId = conditionalUniqueId;

		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			mappedParameterGroupUniqueId = parameterGroupUniqueId + "_" + boost::uuids::to_string(uuid);

			res = module->GetMappedParameterName(parentParameterGroupUniqueId, mappedparentParameterGroupUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->GetMappedParameterName(conditionalUniqueId, mappedconditionalUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->CreateParamMapping(parameterGroupUniqueId, mappedParameterGroupUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<Parameter> conditionParam;
		if (!conditionalUniqueId.empty())
		{
			res = node->GetApplicationProcess()->GetParameter(mappedconditionalUniqueId, conditionParam);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ParameterGroup> ptr(new ParameterGroup(mappedParameterGroupUniqueId, conditionalUniqueId, conditionalValue, conditionParam, bitOffset));
		std::shared_ptr<ParameterGroup> grp;
		res = node->GetApplicationProcess()->GetParameterGroup(mappedparentParameterGroupUniqueId, grp);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = grp->AddParameterGroupEntry(ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->GetApplicationProcess()->AddParameterGroup(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateParameterReference(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& parameterUniqueIdRef, const std::string& actualValue, std::uint16_t bitOffset, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::string mappedParameterGroupUniqueId = parameterGroupUniqueId;
		std::string mappedParameterUniqueIdRef = parameterUniqueIdRef;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->GetMappedParameterName(parameterGroupUniqueId, mappedParameterGroupUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->GetMappedParameterName(parameterUniqueIdRef, mappedParameterUniqueIdRef);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<Parameter> param;
		res = node->GetApplicationProcess()->GetParameter(mappedParameterUniqueIdRef, param);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::make_shared<ParameterReference>(mappedParameterUniqueIdRef, param, actualValue, bitOffset);

		std::shared_ptr<ParameterGroup> grp;
		res = node->GetApplicationProcess()->GetParameterGroup(mappedParameterGroupUniqueId, grp);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = grp->AddParameterGroupEntry(ptr);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::EvaluateParameterGroupCondition(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, bool& conditionMet)
{
	try
	{
		std::string mappedParameterGroupUniqueId = parameterGroupUniqueId;
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (cn && !interfaceId.empty() && !moduleId.empty() && modulePosition != 0)
		{
			std::shared_ptr<Module> module;
			res = cn->GetModule(interfaceId, moduleId, modulePosition, module);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = module->GetMappedParameterName(parameterGroupUniqueId, mappedParameterGroupUniqueId);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		std::shared_ptr<ParameterGroup> grp;
		res = node->GetApplicationProcess()->GetParameterGroup(mappedParameterGroupUniqueId, grp);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		conditionMet = grp->CheckParameterGroupCondition();
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetDatatypeSize(const std::string& networkId, const std::uint8_t nodeId, const std::string& dataTypeUniqueId, std::uint32_t& size)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ComplexDataType> dt;
		res = node->GetApplicationProcess()->GetComplexDataType(dataTypeUniqueId, dt);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		size = dt->GetBitSize() / 8;
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateDynamicChannel(const std::string& networkId, const std::uint8_t nodeId, PlkDataType dataType, DynamicChannelAccessType accessType, std::uint32_t startIndex, std::uint32_t endIndex, std::uint32_t maxNumber, std::uint32_t addressOffset, std::uint8_t bitAlignment)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto mnPtr = std::dynamic_pointer_cast<ManagingNode>(node);
		if (mnPtr)
		{
			auto ptr = std::shared_ptr<DynamicChannel>(new DynamicChannel(dataType, accessType, startIndex, endIndex, maxNumber, addressOffset, bitAlignment));
			mnPtr->AddDynamicChannel(ptr);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
			return Result(ErrorCode::NODE_IS_NOT_MANAGING_NODE);
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, const std::string& actualValue, bool force, bool validateOnly)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->ForceObject(objectId, force, validateOnly, actualValue);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetSubObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, const std::string& actualValue, bool force, bool validateOnly)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->ForceSubObject(objectId, subObjectId, force, validateOnly, actualValue);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::ResetOperationMode(const std::string& networkId, const std::uint8_t nodeId)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = network->SetOperationMode(nodeId, PlkOperationMode::NORMAL);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetOperationModeChained(const std::string& networkId, const std::uint8_t nodeId)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = network->SetOperationMode(nodeId, PlkOperationMode::CHAINED);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetOperationModeMultiplexed(const std::string& networkId, const std::uint8_t nodeId, const std::uint8_t multiplexedCycle)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = network->SetOperationMode(nodeId, PlkOperationMode::MULTIPLEXED, multiplexedCycle);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::AddNodeAssignment(const std::string& networkId, const std::uint8_t nodeId, const NodeAssignment& assignment)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->AddNodeAssignment(assignment);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::RemoveNodeAssignment(const std::string& networkId, const std::uint8_t nodeId, const NodeAssignment& assignment)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = node->RemoveNodeAssignment(assignment);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::EnableNode(const std::string& networkId, const std::uint8_t nodeID, const bool enable)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (res.IsSuccessful())
			res = network->EnableNode(nodeID, enable);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::string& actualValueReturn)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> obj;
		res = node->GetObject(objectId, obj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (obj->WriteToConfiguration())
		{
			std::string actualValue = "0x";
			actualValue.append(obj->GetTypedActualValue<std::string>());
			actualValueReturn = actualValue;
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		//No actual value present
		boost::format formatter(kMsgObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
		formatter
		% objectId
		% (std::uint32_t) nodeId;
		LOG_ERROR() << formatter.str();
		return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetSubObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, std::string& actualValueReturn)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subobj;
		res = node->GetSubObject(objectId, subObjectId, subobj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (subobj->WriteToConfiguration())
		{
			std::string actualValue = "0x";
			actualValue.append(subobj->GetTypedActualValue<std::string>());
			actualValueReturn = actualValue;
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		//No actual value present
		boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
		formatter
		% objectId
		% (std::uint32_t) subObjectId
		% (std::uint32_t) nodeId;
		LOG_INFO() << formatter.str();
		return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetObjectsWithActualValue(const std::string& networkId, const std::uint8_t nodeId, std::map<std::pair<std::uint32_t, std::int32_t>, std::string>& objects)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		for (auto& obj : node->GetObjectDictionary())
		{
			if (obj.second->HasActualValue())
			{
				auto pair = std::pair<std::uint32_t, std::int32_t>(obj.first, -1);
				objects.insert(std::pair<std::pair<std::uint32_t, std::int32_t>, std::string>(pair, "0x" + obj.second->GetTypedActualValue<std::string>()));
			}

			auto& subod = obj.second->GetSubObjectDictionary();
			for (auto& subobj : subod)
			{
				if (subobj.second->HasActualValue())
				{
					auto pair = std::pair<std::uint32_t, std::int32_t>(obj.first, subobj.first);
					objects.insert(std::pair<std::pair<std::uint32_t, std::int32_t>, std::string>(pair, "0x" + subobj.second->GetTypedActualValue<std::string>()));
				}
			}
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetNodeId(const std::string& networkId, const std::uint8_t nodeId, const std::uint8_t newNodeId)
{
	std::shared_ptr<Network> network;
	Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
	if (!res.IsSuccessful())
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

	res = network->SetNodeId(nodeId, newNodeId);
	return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
}

Result OpenConfiguratorCore::SetNodeName(const std::string& networkId, const std::uint8_t nodeId, const std::string& nodeName)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!nodeName.empty())
			node->SetName(nodeName);

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetAsyncSlotTimeout(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t asyncSlotTimeout)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(node);
		if (ptr)
		{
			std::shared_ptr<SubObject> subObj;
			res = ptr->GetSubObject(0x1F8A, 0x2, subObj);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = ptr->ForceSubObject(0x1F8A, 0x2, subObj->GetForceToCDC(), false, IntToHex(asyncSlotTimeout, 8, "0x"));
		}
		else
		{
			boost::format formatter(kMsgNonManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetAsndMaxNr(const std::string& networkId, const std::uint8_t nodeId, std::uint8_t asndMaxNr)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(node);
		if (ptr)
		{
			std::shared_ptr<SubObject> subObj;
			res = ptr->GetSubObject(0x1F8A, 0x3, subObj);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = ptr->ForceSubObject(0x1F8A, 0x3, subObj->GetForceToCDC(), false, IntToHex((std::uint16_t) asndMaxNr, 2, "0x"));
		}
		else
		{
			boost::format formatter(kMsgNonManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetPResTimeOut(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t presTimeout)
{
	try
	{
		if (presTimeout < 25000)
		{
			boost::format formatter(kMsgLowCnPresTimeoutDefault[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% presTimeout
			% (std::uint32_t) nodeId
			% (std::uint32_t) nodeId;
			LOG_WARN() << "[" + networkId + "] " + formatter.str();
		}

		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> mnPtr;
		res = network->GetManagingNode(mnPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subObj;
		res = mnPtr->GetSubObject(0x1F92, (std::uint32_t) nodeId, subObj);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = mnPtr->ForceSubObject(0x1F92, (std::uint32_t) nodeId, subObj->GetForceToCDC(), false, IntToHex(presTimeout, 8, "0x"));
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetRedundantManagingNodeWaitNotActive(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t waitNotActive)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(node);
		if (ptr && nodeId != 240)
		{
			std::shared_ptr<SubObject> subObj;
			res = ptr->GetSubObject(0x1F89, 0x1, subObj);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = ptr->ForceSubObject(0x1F89, 0x1, subObj->GetForceToCDC(), false, IntToHex(waitNotActive, 8, "0x"));
		}
		else
		{
			boost::format formatter(kMsgNonRedundantManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_REDUNDANT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetRedundantManagingNodePriority(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t priority)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(node);
		if (ptr && nodeId != 240)
		{
			std::shared_ptr<SubObject> subObj;
			res = ptr->GetSubObject(0x1F89, 0xA, subObj);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			res = ptr->ForceSubObject(0x1F89, 0xA, subObj->GetForceToCDC(), false, IntToHex(priority, 8, "0x"));
		}
		else
		{
			boost::format formatter(kMsgNonRedundantManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_REDUNDANT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetCycleTime(const std::string& networkId, std::uint32_t& cycleTime)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> object;
		res = nodePtr->GetObject(0x1006, object);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		if (object->WriteToConfiguration())
			cycleTime = object->GetTypedActualValue<std::uint32_t>();
		else
		{
			boost::format formatter(kMsgObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) 0x1006
			% (std::uint32_t) 240;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetAsyncMtu(const std::string& networkId, std::uint16_t& asyncMtu)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subobject;
		res = nodePtr->GetSubObject(0x1F98, 0x8, subobject);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (subobject->WriteToConfiguration())
			asyncMtu = subobject->GetTypedActualValue<std::uint16_t>();
		else
		{
			boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) 0x1F98
			% (std::uint32_t) 0x8
			% (std::uint32_t) 240;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetMultiplexedCycleCount(const std::string& networkId, std::uint16_t& multiplexedCycleLength)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subobject;
		res = nodePtr->GetSubObject(0x1F98, 0x7, subobject);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (subobject->WriteToConfiguration())
			multiplexedCycleLength = subobject->GetTypedActualValue<std::uint16_t>();
		else
		{
			boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) 0x1F98
			% (std::uint32_t) 0x7
			% (std::uint32_t) 240;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}
Result OpenConfiguratorCore::GetPrescaler(const std::string& networkId, std::uint16_t& prescaler)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subobject;
		res = nodePtr->GetSubObject(0x1F98, 0x9, subobject);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (subobject->WriteToConfiguration())
			prescaler = subobject->GetTypedActualValue<std::uint16_t>();
		else
		{
			boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) 0x1F98
			% (std::uint32_t) 0x9
			% (std::uint32_t) 240;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetAsyncSlotTimeout(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& asyncSlotTimeout)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(nodePtr);
		if (ptr)
		{
			std::shared_ptr<SubObject> subobject;
			res = nodePtr->GetSubObject(0x1F8A, 0x2, subobject);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			if (subobject->WriteToConfiguration())
				asyncSlotTimeout = subobject->GetTypedActualValue<std::uint32_t>();
			else
			{
				boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
				formatter
				% (std::uint32_t) 0x1F8A
				% (std::uint32_t) 0x2
				% (std::uint32_t) 240;
				LOG_ERROR() << formatter.str();
				return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
			}
		}
		else
		{
			boost::format formatter(kMsgNonManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetAsndMaxNr(const std::string& networkId, const std::uint8_t nodeId, std::uint16_t& asndMaxNr)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(nodePtr);
		if (ptr)
		{
			std::shared_ptr<SubObject> subobject;
			res = nodePtr->GetSubObject(0x1F8A, 0x3, subobject);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			if (subobject->WriteToConfiguration())
				asndMaxNr = subobject->GetTypedActualValue<std::uint16_t>();
			else
			{
				boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
				formatter
				% (std::uint32_t) 0x1F8A
				% (std::uint32_t) 0x3
				% (std::uint32_t) 240;
				LOG_ERROR() << formatter.str();
				return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
			}
		}
		else
		{
			boost::format formatter(kMsgNonManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetPResTimeOut(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& presTimeout)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> subobject;
		res = nodePtr->GetSubObject(0x1F92, (std::uint32_t) nodeId, subobject);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (subobject->HasActualValue())
			presTimeout = subobject->GetTypedActualValue<std::uint32_t>();
		else if (subobject->HasDefaultValue())
			presTimeout = subobject->GetTypedDefaultValue<std::uint32_t>();
		else
		{
			boost::format formatter(kMsgDefaultCnPresTimeout[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::SUB_OBJECT_HAS_NO_DEFAULT_VALUE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetRedundantManagingNodeWaitNotActive(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& waitNotActive)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(nodePtr);
		if (ptr && nodeId != 240)
		{
			std::shared_ptr<SubObject> subobject;
			res = nodePtr->GetSubObject(0x1F89, 0x1, subobject);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			if (subobject->WriteToConfiguration())
				waitNotActive = subobject->GetTypedActualValue<std::uint32_t>();
			else
			{
				boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
				formatter
				% (std::uint32_t) 0x1F89
				% (std::uint32_t) 0x1
				% (std::uint32_t) 240;
				LOG_ERROR() << formatter.str();
				return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
			}
		}
		else
		{
			boost::format formatter(kMsgNonRedundantManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_REDUNDANT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetRedundantManagingNodePriority(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& priority)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> nodePtr;
		res = networkPtr->GetManagingNode(nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto ptr = std::dynamic_pointer_cast<ManagingNode>(nodePtr);
		if (ptr && nodeId != 240)
		{
			std::shared_ptr<SubObject> subobject;
			res = nodePtr->GetSubObject(0x1F89, 0xA, subobject);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

			if (subobject->WriteToConfiguration())
				priority = subobject->GetTypedActualValue<std::uint32_t>();
			else
			{
				boost::format formatter(kMsgSubObjectNoActualValue[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
				formatter
				% (std::uint32_t) 0x1F89
				% (std::uint32_t) 0xA
				% (std::uint32_t) 240;
				LOG_ERROR() << formatter.str();
				return Result(ErrorCode::OBJECT_HAS_NO_ACTUAL_VALUE, "[" + networkId + "] " + formatter.str());
			}
		}
		else
		{
			boost::format formatter(kMsgNonRedundantManagingNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_REDUNDANT_MANAGING_NODE, "[" + networkId + "] " + formatter.str());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetNodeAssignment(const std::string& networkId, const std::uint8_t nodeID, std::vector<std::uint32_t>& assignmentColl)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeID, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		for (auto assign : nodePtr->GetNodeAssignment())
		{
			assignmentColl.push_back(static_cast<std::underlying_type<NodeAssignment>::type>(assign));
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetLossOfSocTolerance(const std::string& networkId, const std::uint8_t, const std::uint32_t lossOfSocTolerance)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		res = network->SetLossOfSoCTolerance(lossOfSocTolerance);
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetLossOfSocTolerance(const std::string& networkId, const std::uint8_t, std::uint32_t& lossOfSocTolerance)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		lossOfSocTolerance = networkPtr->GetLossOfSoCTolerance();
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::MapAllObjectsToChannel(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, bool updateNrEntries)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto cn = std::dynamic_pointer_cast<ControlledNode>(nodePtr);
		if (cn)
		{
			if (dir == Direction::RX)
			{
				res = cn->MapAllRxObjects(channelNr, updateNrEntries);
				if (!res.IsSuccessful())
					return Result(res.GetErrorType(), "[" + networkId + "] " + "Insufficient mapping objects available : " + res.GetErrorMessage());
				else
					return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			}
			else if (dir == Direction::TX)
			{
				res = cn->MapAllTxObjects(channelNr, updateNrEntries);
				if (!res.IsSuccessful())
					return Result(res.GetErrorType(), "[" + networkId + "] " + "Insufficient mapping objects available : " + res.GetErrorMessage());
				else
					return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			}
		}

		return Result(ErrorCode::NODE_IS_NOT_CONTROLLED_NODE, "[" + networkId + "] " + kMsgMappingOperationNotSupported[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::MapObjectToChannel(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, std::uint16_t position, std::uint32_t objectIdToBeMapped, std::uint16_t fromNode, bool updateNrEntries)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto cn = std::dynamic_pointer_cast<ControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->MapObject(objectIdToBeMapped, dir, channelNr, position, fromNode, updateNrEntries);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
			return Result(ErrorCode::NODE_IS_NOT_CONTROLLED_NODE, "[" + networkId + "] " + kMsgMappingOperationNotSupported[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);

	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::MapSubObjectToChannel(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, std::uint16_t position, std::uint32_t objectIdToBeMapped, std::uint16_t suObjectIdToBeMapped, std::uint16_t fromNode, bool updateNrEntries)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto cn = std::dynamic_pointer_cast<ControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->MapSubObject(objectIdToBeMapped, suObjectIdToBeMapped, dir, channelNr, position, fromNode, updateNrEntries);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
			return Result(ErrorCode::NODE_IS_NOT_CONTROLLED_NODE, "[" + networkId + "] " + kMsgMappingOperationNotSupported[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetChannelSize(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, std::uint32_t& size)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		std::uint32_t channelObjectId = 0;

		if (dir == Direction::RX)
			channelObjectId = 0x1600 + channelNr;
		else if (dir == Direction::TX)
			channelObjectId = 0x1A00 + channelNr;

		std::shared_ptr<Object> mappingChannel;
		res = nodePtr->GetObject(channelObjectId, mappingChannel);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::uint16_t nrOfEntries = 0;
		for (auto& channelObj : mappingChannel->GetSubObjectDictionary())
		{
			if (channelObj.first == 0)
			{
				if (channelObj.second->HasActualValue())
				{
					nrOfEntries = channelObj.second->GetTypedActualValue<std::uint16_t>();
				}
				else if (channelObj.second->HasDefaultValue())
				{
					nrOfEntries = channelObj.second->GetTypedDefaultValue<std::uint16_t>();
				}
				continue;
			}

			if (nrOfEntries == 0)
				return Result();

			if (channelObj.second->HasActualValue())
			{
				std::uint64_t mappingValue = channelObj.second->GetTypedActualValue<std::uint64_t>();
				if (mappingValue != 0)
				{
					BaseProcessDataMapping mapping = BaseProcessDataMapping(channelObj.second->GetTypedActualValue<std::string>(), 0);
					size += mapping.GetMappingLength() / 8;
					nrOfEntries--;
				}
			}
			else if (channelObj.second->HasDefaultValue())
			{
				std::uint64_t mappingValue = channelObj.second->GetTypedDefaultValue<std::uint64_t>();
				if (mappingValue != 0)
				{
					BaseProcessDataMapping mapping = BaseProcessDataMapping(channelObj.second->GetTypedDefaultValue<std::string>(), 0);
					size += mapping.GetMappingLength() / 8;
					nrOfEntries--;
				}
			}
			else
				continue;
		}

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetChannelActualValues(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, std::map<std::pair<std::uint32_t, std::int32_t>, std::string>& objects)
{
	try
	{
		//Get the network
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		//Get the node
		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		//Calculate the channel object index
		std::uint32_t channelObjectId = 0;

		if (dir == Direction::RX)
			channelObjectId = 0x1600 + channelNr;
		else if (dir == Direction::TX)
			channelObjectId = 0x1A00 + channelNr;

		std::shared_ptr<Object> mappingChannel;
		res = nodePtr->GetObject(channelObjectId, mappingChannel);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		for (auto& subobj : mappingChannel->GetSubObjectDictionary())
		{
			if (subobj.second->HasActualValue())
			{
				auto pair = std::pair<std::uint32_t, std::int32_t>(channelObjectId, subobj.first);
				objects.insert(std::pair<std::pair<std::uint32_t, std::int32_t>, std::string>(pair, "0x" + subobj.second->GetTypedActualValue<std::string>()));
			}
			else if (subobj.second->HasDefaultValue())
			{
				auto pair = std::pair<std::uint32_t, std::int32_t>(channelObjectId, subobj.first);
				objects.insert(std::pair<std::pair<std::uint32_t, std::int32_t>, std::string>(pair, "0x" + subobj.second->GetTypedDefaultValue<std::string>()));
			}
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::MoveMappingObject(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, std::uint16_t oldPosition, std::uint16_t newPosition)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		auto cn = std::dynamic_pointer_cast<ControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->MoveMappingObject(dir, channelNr, oldPosition, newPosition);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
			return Result(ErrorCode::NODE_IS_NOT_CONTROLLED_NODE, "[" + networkId + "] " + kMsgMappingOperationNotSupported[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::ClearMappingObject(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr, std::uint16_t position)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::uint32_t channelParamId = 0;
		std::uint32_t channelObjectId = 0;

		if (dir == Direction::RX)
		{
			channelObjectId = 0x1600 + channelNr;
			channelParamId = 0x1400 + channelNr;
		}
		else if (dir == Direction::TX)
		{
			channelObjectId = 0x1A00 + channelNr;
			channelParamId = 0x1800 + channelNr;
		}
		std::shared_ptr<SubObject> mappingParam;
		res = nodePtr->GetSubObject(channelParamId, 0x1, mappingParam);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		if (mappingParam->HasActualValue())
			mappingParam->ClearActualValue();

		std::shared_ptr<Object> mappingChannel;
		res = nodePtr->GetObject(channelObjectId, mappingChannel);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		for (auto& channelObj : mappingChannel->GetSubObjectDictionary())
		{
			if (channelObj.first == position)
			{
				if (channelObj.second->HasActualValue())
					channelObj.second->ClearActualValue();
				break;
			}
		}

		auto cn = std::dynamic_pointer_cast<ControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->UpdateProcessImage(dir);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		return Result();
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::ClearMappingChannel(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint16_t channelNr)
{
	try
	{
		std::shared_ptr<Network> networkPtr;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, networkPtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = networkPtr->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::uint32_t channelParamId = 0;
		std::uint32_t channelObjectId = 0;

		if (dir == Direction::RX)
		{
			channelObjectId = 0x1600 + channelNr;
			channelParamId = 0x1400 + channelNr;
		}
		else if (dir == Direction::TX)
		{
			channelObjectId = 0x1A00 + channelNr;
			channelParamId = 0x1800 + channelNr;
		}
		std::shared_ptr<SubObject> mappingParam;
		res = nodePtr->GetSubObject(channelParamId, 0x1, mappingParam);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		if (mappingParam->HasActualValue())
			mappingParam->ClearActualValue();

		std::shared_ptr<Object> mappingChannel;
		res = nodePtr->GetObject(channelObjectId, mappingChannel);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		for (auto& channelObj : mappingChannel->GetSubObjectDictionary())
		{
			if (channelObj.second->HasActualValue())
				channelObj.second->ClearActualValue();
		}
		return res;
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateModuleObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet, const std::string& rangeSelector)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> modularHead = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (!modularHead)
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}

		std::shared_ptr<Object> ptr(new Object(objectId, objectType, name, nodeId));

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = modularHead->AddObjectToModule(interfaceId, modulePosition, moduleId, rangeSelector, objectId, ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = modularHead->ForceObject(objectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateModuleParameterObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet, const std::string& rangeSelector)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> modularHead = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (!modularHead)
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}

		std::shared_ptr<Module> module;
		modularHead->GetModule(interfaceId, moduleId, modulePosition, module);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::string mappedUniqueIdRef;
		res = module->GetMappedParameterName(uniqueIdRef, mappedUniqueIdRef);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<Object> ptr(new Object(objectId, objectType, name, nodeId));

		std::shared_ptr<Parameter> param;
		res = modularHead->GetApplicationProcess()->GetParameter(mappedUniqueIdRef, param);
		if (!res.IsSuccessful())
		{
			std::shared_ptr<ParameterGroup> paramGrp;
			res = modularHead->GetApplicationProcess()->GetParameterGroup(mappedUniqueIdRef, paramGrp);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			else
				ptr->SetComplexDataType(paramGrp);
		}
		else
		{
			ptr->SetComplexDataType(param);
		}

		ptr->SetUniqueIdRef(mappedUniqueIdRef);

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);
		else if (dataType == PlkDataType::UNDEFINED && !uniqueIdRef.empty())
			ptr->SetDataType(PlkDataType::Domain);
		else
		{
			boost::format formatter(kMsgBaseObjectDataTypeError[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% name
			% objectId
			% (std::uint32_t) nodeId;
			LOG_FATAL() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_DATATYPE, formatter.str());
		}

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = modularHead->AddObjectToModule(interfaceId, modulePosition, moduleId, rangeSelector, objectId, ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = modularHead->ForceObject(objectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateModuleSubObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, std::uint16_t subObjectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> modularHead = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (!modularHead)
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}

		std::shared_ptr<SubObject> ptr(new SubObject(subObjectId, objectType, name, nodeId));

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = modularHead->AddSubObjectToModule(interfaceId, modulePosition, moduleId, objectId, subObjectId, ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = node->ForceSubObject(objectId, subObjectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateModuleParameterSubObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, std::uint16_t subObjectId, ObjectType objectType, const std::string& name, PlkDataType dataType, AccessType accessType, PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> node;
		res = network->GetBaseNode(nodeId, node);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> modularHead = std::dynamic_pointer_cast<ModularControlledNode>(node);
		if (!modularHead)
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}

		std::shared_ptr<Module> module;
		modularHead->GetModule(interfaceId, moduleId, modulePosition, module);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::string mappedUniqueIdRef;
		res = module->GetMappedParameterName(uniqueIdRef, mappedUniqueIdRef);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<SubObject> ptr(new SubObject(subObjectId, objectType, name, nodeId));

		std::shared_ptr<Parameter> param;
		res = modularHead->GetApplicationProcess()->GetParameter(mappedUniqueIdRef, param);
		if (!res.IsSuccessful())
		{
			std::shared_ptr<ParameterGroup> paramGrp;
			res = modularHead->GetApplicationProcess()->GetParameterGroup(mappedUniqueIdRef, paramGrp);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
			else
				ptr->SetComplexDataType(paramGrp);
		}
		else
		{
			ptr->SetComplexDataType(param);
		}

		ptr->SetUniqueIdRef(mappedUniqueIdRef);

		if (dataType != PlkDataType::UNDEFINED)
			ptr->SetDataType(dataType);
		else if (dataType == PlkDataType::UNDEFINED && !uniqueIdRef.empty())
			ptr->SetDataType(PlkDataType::Domain);
		else
		{
			boost::format formatter(kMsgBaseObjectDataTypeError[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% name
			% objectId
			% (std::uint32_t) nodeId;
			LOG_FATAL() << formatter.str();
			return Result(ErrorCode::OBJECT_HAS_NO_DATATYPE, formatter.str());
		}

		if (pdoMapping != PDOMapping::UNDEFINED)
			ptr->SetPDOMapping(pdoMapping);

		if (accessType != AccessType::UNDEFINED)
			ptr->SetAccessType(accessType);

		if (!defaultValueToSet.empty())
		{
			res = ptr->SetTypedObjectDefaultValue(defaultValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}

		res = modularHead->AddSubObjectToModule(interfaceId, modulePosition, moduleId, objectId, subObjectId, ptr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (!actualValueToSet.empty())
		{
			res = modularHead->ForceSubObject(objectId, subObjectId, false, false, actualValueToSet);
			if (!res.IsSuccessful())
				return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateModularHeadNode(const std::string& networkId, const std::uint8_t nodeId, const std::string& nodeName)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		if (nodeId >= 1 && nodeId <= 239)
		{
			std::shared_ptr<ModularControlledNode> node = std::make_shared<ModularControlledNode>(nodeId, nodeName);
			res = network->AddNode(node);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			//Nodeid invalid
			boost::format formatter(kMsgNodeIdInvalid[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODEID_INVALID, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateInterface(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, const std::string& type, ModuleAddressing moduleAddressing, std::uint32_t maxModules, bool unusedSlots, bool multipleModules)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->AddInterface(uniqueId, type, moduleAddressing, maxModules, unusedSlots, multipleModules);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateRange(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& name, std::uint32_t baseIndex, std::uint32_t maxIndex, std::uint32_t maxSubIndex, std::uint32_t sortStep, SortMode sortMode, SortNumber sortNumber, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->AddRange(interfaceId, name, baseIndex, maxIndex, maxSubIndex, sortStep, sortMode, sortNumber, pdoMapping);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::CreateModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t moduleAddress, const std::string& moduleType, const std::string& moduleName, ModuleAddressing moduleAddressing, std::uint16_t minPosition, std::uint16_t maxPosition, std::uint16_t minAddress, std::uint16_t maxAddress, std::uint16_t maxCount)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->AddModule(interfaceId, moduleId, moduleType, moduleAddressing, modulePosition, moduleAddress, moduleName, minPosition, maxPosition, minAddress, maxAddress, maxCount);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::RemoveModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->RemoveModule(interfaceId, moduleId, modulePosition);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::EnableModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, bool enable)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->EnableModule(interfaceId, moduleId, modulePosition, enable);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::MoveModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t oldPosition, std::uint32_t newPosition)
{
	try
	{
		if (oldPosition == newPosition)
			return Result();

		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->ChangeModuleOrderOnInterface(interfaceId, moduleId, oldPosition, newPosition);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetModuleObjectCurrentIndex(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t position, std::uint32_t originalIndex, std::int32_t originalSubIndex, std::uint32_t& returnIndex, std::int32_t& returnSubIndex)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->GetModuleObjectCurrentIndex(interfaceId, moduleId, position, originalIndex, originalSubIndex, returnIndex, returnSubIndex);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetModuleParameterCurrentName(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t position, const std::string& originalParamName, std::string& parameterName)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->GetParameterCurrentName(interfaceId, moduleId, position, originalParamName, parameterName);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::SetModuleAddress(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t position, std::uint32_t address)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<BaseNode> nodePtr;
		res = network->GetBaseNode(nodeId, nodePtr);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ModularControlledNode> cn = std::dynamic_pointer_cast<ModularControlledNode>(nodePtr);
		if (cn)
		{
			res = cn->SetModuleAddress(interfaceId, moduleId, position, address);
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());
		}
		else
		{
			boost::format formatter(kMsgNodeIsNotAModularControlledNode[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
			formatter
			% (std::uint32_t) nodeId;
			LOG_ERROR() << formatter.str();
			return Result(ErrorCode::NODE_IS_NOT_MODULAR_CONTROLLED_NODE, "[" + networkId + "] " + formatter.str());
		}
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetMappingObjectProcessImageOffset(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, std::uint32_t objectId, std::uint32_t subObjectId,  std::uint32_t& offset)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> mn;
		res = network->GetManagingNode(mn);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::vector<std::shared_ptr<BaseProcessImageObject>> piCollection;
		if (dir == Direction::RX)
			piCollection = mn->GetReceiveProcessImage();
		else if (dir == Direction::TX)
			piCollection = mn->GetTransmitProcessImage();

		std::uint32_t piSize = 0;
		for (auto& piObj : piCollection)
		{
			// Align the PI to 8 bit, 16 bit and 32 bit
			if (piObj->GetSize() % 8 == 0 || piObj->GetSize() % 16 == 0 || piObj->GetSize() % 32 == 0)
			{
				if (piSize % piObj->GetSize() != 0)
				{
					std::uint32_t paddingSize = piObj->GetSize() - (piSize % piObj->GetSize());
					piSize += paddingSize;
				}
			}
			if (piObj->GetSourceNodeId() == (std::uint16_t) nodeId &&
			        piObj->GetMappingObjectIndex() == objectId &&
			        piObj->GetMappingObjectSubIndex() == subObjectId)
			{
				//Return bitoffset
				offset = piSize;
				return Result();
			}
			piSize += piObj->GetSize();
		}
		boost::format formatter(kMsgNonExistingMappedSubObject[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
		formatter
		% objectId
		% subObjectId
		% (std::uint32_t) nodeId;
		LOG_ERROR() << formatter.str();
		return Result(ErrorCode::MAPPED_OBJECT_DOES_NOT_EXIST, "[" + networkId + "] " + formatter.str());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}

Result OpenConfiguratorCore::GetMappingParameterProcessImageOffset(const std::string& networkId, const std::uint8_t nodeId, const Direction& dir, const std::string& parameterName, std::uint32_t& offset)
{
	try
	{
		std::shared_ptr<Network> network;
		Result res = ProjectManager::GetInstance().GetNetwork(networkId, network);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::shared_ptr<ManagingNode> mn;
		res = network->GetManagingNode(mn);
		if (!res.IsSuccessful())
			return Result(res.GetErrorType(), "[" + networkId + "] " + res.GetErrorMessage());

		std::vector<std::shared_ptr<BaseProcessImageObject>> piCollection;
		if (dir == Direction::RX)
			piCollection = mn->GetReceiveProcessImage();
		else if (dir == Direction::TX)
			piCollection = mn->GetTransmitProcessImage();

		std::uint32_t piSize = 0;
		for (auto& piObj : piCollection)
		{
			// Align the PI to 8 bit, 16 bit and 32 bit
			if (piObj->GetSize() % 8 == 0 || piObj->GetSize() % 16 == 0 || piObj->GetSize() % 32 == 0)
			{
				if (piSize % piObj->GetSize() != 0)
				{
					std::uint32_t paddingSize = piObj->GetSize() - (piSize % piObj->GetSize());
					piSize += paddingSize;
				}
			}
			if (piObj->GetSourceNodeId() == (std::uint16_t) nodeId &&
			        piObj->GetMappingObjectParameter() == parameterName)
			{
				//Return bitoffset
				offset = piSize;
				return Result();
			}
			piSize += piObj->GetSize();
		}
		boost::format formatter(kMsgParameterNotFound[static_cast<std::underlying_type<Language>::type>(LoggingConfiguration::GetInstance().GetCurrentLanguage())]);
		formatter
		% parameterName;
		LOG_ERROR() << formatter.str();
		return Result(ErrorCode::PARAMETER_NOT_FOUND, "[" + networkId + "] " + formatter.str());
	}
	catch (const std::exception& ex)
	{
		return Result(ErrorCode::UNHANDLED_EXCEPTION, "[" + networkId + "] " + ex.what());
	}
}
