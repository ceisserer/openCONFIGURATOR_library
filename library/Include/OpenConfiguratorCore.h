/************************************************************************
\file OpenConfiguratorCore.h
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
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/
#if !defined OPEN_CONFIGURATOR_CORE_H
#define OPEN_CONFIGURATOR_CORE_H

#include <map>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "BaseNode.h"
#include "Constants.h"
#include "ProjectManager.h"
#include "Network.h"
#include "PlkDataType.h"
#include "AccessType.h"
#include "DynamicChannelAccessType.h"
#include "LoggingConfiguration.h"
#include "IEC_Datatype.h"
#include "ObjectType.h"
#include "StructDataType.h"
#include "ArrayDataType.h"
#include "EnumDataType.h"
#include "DynamicChannel.h"
#include "ConfigurationGenerator.h"
#include "Utilities.h"
#include "XmlProcessImageGenerator.h"
#include "NetProcessImageGenerator.h"
#include "CProcessImageGenerator.h"
#include "Direction.h"
#include "SortEnums.h"
#include "ModularControlledNode.h"

/**
\brief Industrial network root namespace
\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
*/
namespace IndustrialNetwork
{
	/**
	\brief %POWERLINK related namespace
	\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
	*/
	namespace POWERLINK
	{
		/**
		\brief %Core library related namespace
		\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
		*/
		namespace Core
		{
			/**
			\brief %API related namespace
			\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
			*/
			namespace API
			{
				/**
				\brief Represents the library %API.
				\author rueckerc, Bernecker+Rainer Industrie Elektronik Ges.m.b.H.
				*/
				class DLLEXPORT OpenConfiguratorCore
				{

					public:
						virtual ~OpenConfiguratorCore();

						static OpenConfiguratorCore& GetInstance();

						/** \addtogroup library
						* @{
						*/
						//! Member to pass the Boost logging configuration to the library.
						/*!
						The library will initialize the logging with the given configuration.
						\param configuration %Configuration for the Boost logging.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result InitLoggingConfiguration(const std::string& configuration);
						//! Member to pass the logging path for the eclipse style logging.
						/*!
						The library will initialize the logging path with an eclipse style configuration.
						\param loggingPath Path for the logging output.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result InitEclipseLoggingPath(const std::string& loggingPath);
						//! Member to set the logging language of the library.
						/*!
						The library will initialize the logging with the passed language.
						\param lang Logging language.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetLoggingLanguage(const IndustrialNetwork::POWERLINK::Core::CoreConfiguration::Language& lang);
						/** @} */

						/** \addtogroup network
						* @{
						*/
						//! Member to initialize a new %POWERLINK network.
						/*!
						The library will create an internal structure to handle the new network configuration. The network identifier must be unique to distinguish from existing networks.
						\param networkId Specifies the identifier for the new network.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateNetwork(const std::string& networkId);
						//! Member to remove a %POWERLINK network from the library.
						/*!
						The library will delete the internal structure of the network configuration.
						\param networkId Specifies the identifier for the network.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result RemoveNetwork(const std::string& networkId);
						//! Member to get the reference to a %POWERLINK network from the library.
						/*!
						The library will return a reference to the internal structure of the network configuration.
						\param networkId Specifies the identifier for the network.
						\param network Reference to the existing network.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetNetwork(const std::string& networkId, IndustrialNetwork::POWERLINK::Core::NetworkHandling::Network& network);
						//! Member to return all existing %POWERLINK network identifier from the library.
						/*!
						The library will return a vector of network identifier strings.
						\param networkList Specifies the return reference to the vector.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetNetworkIds(std::vector<std::string>& networkList);
						//! Member to remove all existing %POWERLINK networks from the library.
						/*!
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result ClearNetworks();
						//! Member to set the %POWERLINK cycle time for the specified network.
						/*!
						This set the managing node object 0x1006.
						\param networkId Specifies the identifier for the network.
						\param cycleTime Cycle time in ys.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetCycleTime(const std::string& networkId, std::uint32_t cycleTime);
						//! Member to set the asynchronous MTU for the specified network.
						/*!
						This set the managing node object 0x1F98 / 0x8.
						\param networkId Specifies the identifier for the network.
						\param asyncMtu Asynchronous MTU in bytes.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetAsyncMtu(const std::string& networkId, std::uint16_t asyncMtu);
						//! Member to set the multiplexed cycle length for the specified network.
						/*!
						This set the managing node object 0x1F98 / 0x7.
						\param networkId Specifies the identifier for the network.
						\param multiplexedCycleLength Multiplexed cycle length.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetMultiplexedCycleCount(const std::string& networkId, std::uint16_t multiplexedCycleLength);
						//! Member to set the prescaler for the specified network.
						/*!
						This set the managing node object 0x1F98 / 0x9.
						\param networkId Specifies the identifier for the network.
						\param prescaler Prescaler.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetPrescaler(const std::string& networkId, std::uint16_t prescaler);
						//! Member to retrieve the %POWERLINK cycle time for the specified network.
						/*!
						This set the managing node object 0x1006.
						\param networkId Specifies the identifier for the network.
						\param cycleTime Cycle time reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetCycleTime(const std::string& networkId, std::uint32_t& cycleTime);
						//! Member to retrieve the asynchronous MTU for the specified network.
						/*!
						This set the managing node object 0x1F98 / 0x8.
						\param networkId Specifies the identifier for the network.
						\param asyncMtu Asynchronous MTU reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetAsyncMtu(const std::string& networkId, std::uint16_t& asyncMtu);
						//! Member to retrieve the multiplexed cycle length for the specified network.
						/*!
						This set the managing node object 0x1F98 / 0x7.
						\param networkId Specifies the identifier for the network.
						\param multiplexedCycleLength Multiplexed cycle reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetMultiplexedCycleCount(const std::string& networkId, std::uint16_t& multiplexedCycleLength);
						//! Member to retrieve the prescaler for the specified network.
						/*!
						This set the managing node object 0x1F98 / 0x9.
						\param networkId Specifies the identifier for the network.
						\param prescaler Prescaler reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetPrescaler(const std::string& networkId, std::uint16_t& prescaler);

						/** @} */

						/** \addtogroup prop
						* @{
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetNodeId(const std::string& networkId, const std::uint8_t nodeId, const std::uint8_t newNodeId);
						//! Member to set the name of a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param nodeName Specifies the node name reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetNodeName(const std::string& networkId, const std::uint8_t nodeId, const std::string& nodeName);
						//! Member to set the async slot timeout for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param asyncSlotTimeout Async slot timeout to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetAsyncSlotTimeout(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t asyncSlotTimeout);
						//! Member to set asnd max number for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param asndMaxNr Asnd max number to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetAsndMaxNr(const std::string& networkId, const std::uint8_t nodeId, const std::uint8_t asndMaxNr);
						//! Member to set PRes timeout for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param presTimeout PRes timeout to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetPResTimeOut(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t presTimeout);
						//! Member to set LossOfSoC tolerance for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier. (not used because common in the complete network)
						\param lossOfSocTolerance LossOfSoC tolerance to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetLossOfSocTolerance(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t lossOfSocTolerance);
						//! Member to set WaitNotActive for a redundant managing node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param waitNotActive WaitNotActive to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetRedundantManagingNodeWaitNotActive(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t waitNotActive);
						//! Member to set node priority for a redundant managing node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param priority Priority to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetRedundantManagingNodePriority(const std::string& networkId, const std::uint8_t nodeId, const std::uint32_t priority);
						//! Member to retrieve the async slot timeout for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param asyncSlotTimeout Async slot timeout reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetAsyncSlotTimeout(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& asyncSlotTimeout);
						//! Member to retrieve asnd max number for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param asndMaxNr Asnd max number reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetAsndMaxNr(const std::string& networkId, const std::uint8_t nodeId, std::uint16_t& asndMaxNr);
						//! Member to retrieve PRes timeout for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param presTimeout PRes timeout reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetPResTimeOut(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& presTimeout);
						//! Member to retrieve LossOfSoC tolerance for a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier. (not used because common in the complete network)
						\param lossOfSocTolerance LossOfSoC tolerance reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetLossOfSocTolerance(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& lossOfSocTolerance);
						//! Member to retrieve WaitNotActive for a redundant managing node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param waitNotActive WaitNotActive reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetRedundantManagingNodeWaitNotActive(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& waitNotActive);
						//! Member to retrieve node priority for a redundant managing node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param priority Priority reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetRedundantManagingNodePriority(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t& priority);
						//! Member to add a node assignement to a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeID Specifies the node identifier.
						\param assignment NodeAssignment to add to the node.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result AddNodeAssignment(const std::string& networkId, const std::uint8_t nodeID, const IndustrialNetwork::POWERLINK::Core::Node::NodeAssignment& assignment);
						//! Member to remove a node assignement from a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeID Specifies the node identifier.
						\param assignment NodeAssignment to be removed from a node.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result RemoveNodeAssignment(const std::string& networkId, const std::uint8_t nodeID, const IndustrialNetwork::POWERLINK::Core::Node::NodeAssignment& assignment);
						//! Member to return a vector of all the node assignement values of a specific node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeID Specifies the node identifier.
						\param assignmentColl NodeAssignments from the node.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetNodeAssignment(const std::string& networkId, const std::uint8_t nodeID, std::vector<std::uint32_t>& assignmentColl);
						/** @} */

						/** \addtogroup build
						* @{
						*/
						//! Member to return the %POWERLINK configuration in textual and binary format.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configurationOutput %POWERLINK configuration in textual format.
						\param binOutput %POWERLINK configuration in binary format (byte vector).
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result BuildConfiguration(const std::string& networkId, std::string& configurationOutput, std::vector<std::uint8_t>& binOutput);
						//! Member to return the XML process image for a node in the network (usually the managing node).
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeid Specifies the node identifier.
						\param processImageXMLOutput Process image in XML format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result BuildXMLProcessImage(const std::string& networkId, const std::uint8_t nodeid, std::string& processImageXMLOutput);
						//! Member to return the .NET process image for a node in the network (usually the managing node).
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeid Specifies the node identifier.
						\param processImageNETOutput Process image in .NET format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result BuildNETProcessImage(const std::string& networkId, const std::uint8_t nodeid, std::string& processImageNETOutput);
						//! Member to return the C/C++ process image for a node in the network (usually the managing node).
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeid Specifies the node identifier.
						\param processImageCOutput Process image in C/C++ format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result BuildCProcessImage(const std::string& networkId, const std::uint8_t nodeid, std::string& processImageCOutput);
						/** @} */

						/** \addtogroup node
						* @{
						*/
						//! Member to create a new node in the network.
						/*!
						Node can be a controlled node (1 - 239) or a redundant managing node (241 - 249).
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param nodeName Specifies the node name.
						\param isRmn RMN flag.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateNode(const std::string& networkId, const std::uint8_t nodeId, const std::string& nodeName, const bool isRmn = false);
						//! Member to remove a node from the network.
						/*!
						Node can be a controlled node (1 - 239) or a redundant managing node (241 - 249).
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result RemoveNode(const std::string& networkId, const std::uint8_t nodeId);
						//! Member to return a node reference from the network.
						/*!
						Node can be a controlled node (1 - 239) or a redundant managing node (241 - 249).
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param node Node reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetControlledNode(const std::string& networkId, const std::uint8_t nodeId, IndustrialNetwork::POWERLINK::Core::Node::ControlledNode& node);
						//! Member to return the managing node (240) reference from the network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param node Managing node reference.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetManagingNode(const std::string& networkId, IndustrialNetwork::POWERLINK::Core::Node::ManagingNode& node);
						//! Member to return a collection of all existing node identifiers in a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeIdCollection Node identifier vector.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetAvailableNodeIds(const std::string& networkId, std::vector<std::uint8_t>& nodeIdCollection);
						//! Member to enable/disable a node in the network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param enable Enable/Disable flag.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result EnableNode(const std::string& networkId, const std::uint8_t nodeId, const bool enable);
						//! Member to reset the operation mode of a node in the network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result ResetOperationMode(const std::string& networkId, const std::uint8_t nodeId);
						//! Member to set the operation mode of a node in the network to "PRes Chaining".
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetOperationModeChained(const std::string& networkId, const std::uint8_t nodeId);
						//! Member to set the operation mode of a node in the network to "Multiplexing".
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param multiplexedCycle Forced multiplexed cycle.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetOperationModeMultiplexed(const std::string& networkId, const std::uint8_t nodeId, const std::uint8_t multiplexedCycle);
						/** @} */

						/** \addtogroup settings
						* @{
						*/
						//! Member to retrieve the library supported %POWERLINK build configuration settings.
						/*!
						\param supportedSettings Settings collection.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetSupportedSettingIds(std::vector<std::string>& supportedSettings);
						//! Member to enable/disable a configuration setting within a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Specifies the configuration.
						\param settingID Specifies the setting.
						\param enabled Enable/Disable flag.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetConfigurationSettingEnabled(const std::string& networkId, const std::string& configID, const std::string& settingID, bool enabled);
						//! Member to create a new configuration setting for a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Configuration identifier.
						\param name Setting name.
						\param value Setting value.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateConfigurationSetting(const std::string& networkId, const std::string& configID, const std::string& name, const std::string& value);
						//! Member to remove a configuration setting for a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Configuration identifier.
						\param name Setting name.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result RemoveConfigurationSetting(const std::string& networkId, const std::string& configID, const std::string& name);
						//! Member to create a configuration for a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Configuration identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateConfiguration(const std::string& networkId, const std::string& configID);
						//! Member to remove a configuration for a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Configuration identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result RemoveConfiguration(const std::string& networkId, const std::string& configID);
						//! Member to change a configuration name within a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param oldConfigId Old configuration identifier.
						\param newConfigId New configuration identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result ReplaceConfigurationName(const std::string& networkId, const std::string& oldConfigId, const std::string& newConfigId);
						//! Member to retrieve a collection of settings from a configuration.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Configuration identifier.
						\param settings Settings collection.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetConfigurationSettings(const std::string& networkId, const std::string& configID, std::vector<IndustrialNetwork::POWERLINK::Core::Configuration::BuildConfigurationSetting>& settings);
						//! Member to retrieve the active build configuration of a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param activeConfiguration Aktive configuration identifier.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetActiveConfiguration(const std::string& networkId, std::string& activeConfiguration);
						//! Member to set the active build configuration of a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param configID Aktive configuration identifier to set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetActiveConfiguration(const std::string& networkId, const std::string& configID);
						//! Member to retrieve all available build configurations of a network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param bcfgs Build configuration collection.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetBuildConfigurations(const std::string& networkId, std::vector<IndustrialNetwork::POWERLINK::Core::Configuration::PlkConfiguration>& bcfgs);
						/** @} */

						/** \addtogroup od
						* @{
						*/
						//! Member to create an object within a %POWERLINK node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param objectType Specifies the ObjectType of the object.
						\param name Specifies the name of the object.
						\param dataType Specifies the %POWERLINK datatype of the object.
						\param accessType Specifies the access type of the object.
						\param pdoMapping Specifies the PDOmapping attribute of the object.
						\param defaultValueToSet Specifies the default value of the object. (may be empty)
						\param actualValueToSet Specifies the actual value of the object. (may be empty)
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet);
						//! Member to set the high and low limit of an object.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param lowLimit Specifies the low limit of the object actual value.
						\param highLimit Specifies the high limit of the object actual value.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetObjectLimits(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, const std::string& lowLimit, const std::string& highLimit);
						//! Member to create a domain object within a %POWERLINK node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param objectType Specifies the ObjectType of the object.
						\param name Specifies the name of the object.
						\param pdoMapping Specifies the PDOmapping attribute of the object.
						\param uniqueIdRef Specifies the unique identifier reference for the domain object.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateDomainObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& uniqueIdRef);
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameterObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet);

						//! Member to create a subobject within a %POWERLINK node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param subObjectId Specifies the subobject identifier
						\param objectType Specifies the ObjectType of the subobject.
						\param name Specifies the name of the subobject.
						\param dataType Specifies the %POWERLINK datatype of the subobject.
						\param accessType Specifies the access type of the subobject.
						\param pdoMapping Specifies the PDOmapping attribute of the subobject.
						\param defaultValueToSet Specifies the default value of the subobject. (may be empty)
						\param actualValueToSet Specifies the actual value of the subobject. (may be empty)
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateSubObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet);
						//! Member to set the high and low limit of a subobject.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param subObjectId Specifies the subobject identifier
						\param lowLimit Specifies the low limit of the subobject actual value.
						\param highLimit Specifies the high limit of the subobject actual value.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetSubObjectLimits(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, const std::string& lowLimit, const std::string& highLimit);
						//! Member to create a domain subobject within a %POWERLINK node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param subObjectId Specifies the subobject identifier
						\param objectType Specifies the ObjectType of the subobject.
						\param name Specifies the name of the subobject.
						\param pdoMapping Specifies the PDOmapping attribute of the subobject.
						\param uniqueIdRef Specifies the unique identifier reference for the domain object.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateDomainSubObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& uniqueIdRef);
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameterSubObject(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet);
						//! Member to retrieve the data size of a %POWERLINK node object.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param size Reference to data size.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetObjectSize(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint32_t& size);
						//! Member to retrieve the data size of a %POWERLINK node subobject.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object identifier.
						\param subObjectId Specifies the subobject identifier.
						\param size Reference to data size.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetSubObjectSize(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, std::uint32_t& size);
						//! Member to retrieve a structure with all (sub)objects that have actual values.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objects Data structure with all objects and their respektive actual values.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetObjectsWithActualValue(const std::string& networkId, const std::uint8_t nodeId, std::map<std::pair<std::uint32_t, std::int32_t> , std::string>& objects);
						//! Member to retrieve an objects actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object.
						\param actualValueReturn Actual value in string format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::string& actualValueReturn);
						//! Member to retrieve a subobjects actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object.
						\param subObjectId Specifies the subobject.
						\param actualValueReturn Actual value in string format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetSubObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, std::string& actualValueReturn);
						//! Member to set an objects actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object.
						\param actualValue Actual value in string format.
						\param force Configuration force flag for the actual value.
						\param validateOnly Library will not write log entry in case of error.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, const std::string& actualValue, bool force = false, bool validateOnly = false);
						//! Member to set a subobjects actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param objectId Specifies the object.
						\param subObjectId Specifies the subobject.
						\param actualValue Actual value in string format.
						\param force Configuration force flag for the actual value.
						\param validateOnly Library will not write log entry in case of error.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetSubObjectActualValue(const std::string& networkId, const std::uint8_t nodeId, std::uint32_t objectId, std::uint8_t subObjectId, const std::string& actualValue, bool force = false, bool validateOnly = false);
						//! Member to retrieve a %POWERLINK CN feature default value.
						/*!
						This default value is common for all networks.
						\param feature Specifies the CN feature.
						\param defaultValue Return value in string format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetFeatureDefaultValue(IndustrialNetwork::POWERLINK::Core::Node::CNFeatureEnum feature, std::string& defaultValue);
						//! Member to retrieve a %POWERLINK MN feature default value.
						/*!
						This default value is common for all networks.
						\param feature Specifies the MN feature.
						\param defaultValue Return value in string format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetFeatureDefaultValue(IndustrialNetwork::POWERLINK::Core::Node::MNFeatureEnum feature, std::string& defaultValue);
						//! Member to retrieve a %POWERLINK general feature default value.
						/*!
						This default value is common for all networks.
						\param feature Specifies the general feature.
						\param defaultValue Return value in string format.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetFeatureDefaultValue(IndustrialNetwork::POWERLINK::Core::Node::GeneralFeatureEnum feature, std::string& defaultValue);
						//! Member to set a %POWERLINK CN feature actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param feature Specifies the CN feature.
						\param value Actual value for the feature.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetFeatureValue(const std::string& networkId, const std::uint8_t nodeId, IndustrialNetwork::POWERLINK::Core::Node::CNFeatureEnum feature, const std::string& value);
						//! Member to set a %POWERLINK MN feature actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param feature Specifies the MN feature.
						\param value Actual value for the feature.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetFeatureValue(const std::string& networkId, const std::uint8_t nodeId, IndustrialNetwork::POWERLINK::Core::Node::MNFeatureEnum feature, const std::string& value);
						//! Member to set a %POWERLINK general feature actual value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param feature Specifies the general feature.
						\param value Actual value for the feature.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetFeatureValue(const std::string& networkId, const std::uint8_t nodeId, IndustrialNetwork::POWERLINK::Core::Node::GeneralFeatureEnum feature, const std::string& value);
						//! Member to create a dynamic channel for a %POWERLINK node.
						/*!
						Currently only managing and redundant managing node are supporting dynamic channels.
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier (240 and above).
						\param dataType Specifies the %POWERLINK datatype for the dynamic channel.
						\param accessType Specifies the dynamic channel access type for the dynamic channel.
						\param startIndex Specifies the start index for the dynamic channel.
						\param endIndex Specifies the end index for the dynamic channel.
						\param maxNumber Specifies the maximum number of channel objects.
						\param addressOffset Specifies the address offset of the dynamic channel.
						\param bitAlignment Specifies the bit aligment of the dynamic channel (default zero).
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateDynamicChannel(const std::string& networkId, const std::uint8_t nodeId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::DynamicChannelAccessType accessType, std::uint32_t startIndex, std::uint32_t endIndex, std::uint32_t maxNumber, std::uint32_t addressOffset, std::uint8_t bitAlignment = 0);
						/** @} */

						/** \addtogroup mapping
						* @{
						*/
						//! Member to map a nodes data object within the %POWERLINK network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive).
						\param channelNr Specifies the channel number for the mapping.
						\param position Specifies the position within the complete process data image.
						\param objectIdToBeMapped Index of the object to be mapped.
						\param fromNode Defines the cross traffic node for receive mapping.
						\param updateNrEntries Update the number of entries object with the current valid number of mappings.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MapObjectToChannel(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint16_t position, std::uint32_t objectIdToBeMapped, std::uint16_t fromNode, bool updateNrEntries);

						//! Member to map a nodes data subobject within the %POWERLINK network.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive).
						\param channelNr Specifies the channel number for the mapping.
						\param position Specifies the position within the complete process data image.
						\param objectIdToBeMapped Index of the object to be mapped.
						\param suObjectIdToBeMapped Subindex of the object to be mapped.
						\param fromNode Defines the cross traffic node for receive mapping.
						\param updateNrEntries Update the number of entries object with the current valid number of mappings.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MapSubObjectToChannel(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint16_t position, std::uint32_t objectIdToBeMapped, std::uint16_t suObjectIdToBeMapped, std::uint16_t fromNode, bool updateNrEntries);
						//! Member to map all data objects and subobjects on a node in default direction to the managing node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive)
						\param channelNr Specifies the channel number for the mapping.
						\param updateNrEntries Specifies if the number of valid mapping entries should be updated to enable the generated mapping.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MapAllObjectsToChannel(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, bool updateNrEntries);
						//! Member to retrieve the channel data size in bytes.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive)
						\param channelNr Specifies the number XX of the channel (16XX for Rx channels, 1AXX for Tx channels)
						\param size Channel size return value.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetChannelSize(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint32_t& size);
						//! Member to retrieve the channel object actual values.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive)
						\param channelNr Specifies the number XX of the channel (16XX for Rx channels, 1AXX for Tx channels)
						\param objects Return map for the actual values.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetChannelActualValues(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::map<std::pair<std::uint32_t, std::int32_t> , std::string>& objects);
						//! Member to retrieve to move a mapping up or down in the channel.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive)
						\param channelNr Specifies the number XX of the channel (16XX for Rx channels, 1AXX for Tx channels)
						\param oldPosition Position of the mapping to be moved.
						\param newPosition Position to be moved to.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MoveMappingObject(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint16_t oldPosition, std::uint16_t newPosition);
						//! Member to clear a mapping actual value by its channel position.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive)
						\param channelNr Specifies the number XX of the channel (16XX for Rx channels, 1AXX for Tx channels)
						\param position Position of the mapping to be cleared.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result ClearMappingObject(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint16_t position);
						//! Member to clear all actual values of a channel.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive)
						\param channelNr Specifies the number XX of the channel (16XX for Rx channels, 1AXX for Tx channels)
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result ClearMappingChannel(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr);

						//IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateOffsetGap(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint16_t position, std::uint32_t gapSize);

						//IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetOffsetGapSize(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint16_t channelNr, std::uint16_t position, std::uint8_t mappingSubObjectId, std::uint32_t& gapSize);
						/** @} */

						/** \addtogroup applicationprocess
						* @{
						*/
						//! Member to create a parameter with a complex datatype within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueID Specifies the unique id of the parameter.
						\param dataTypeUniqueIDRef Specifies the unique id of a referenced complex datatype.
						\param access Specifies the parameter access type.
						\param createTemplate Create a parameter template not a parameter.
						\param interfaceId Defines interface if parameter is created for a module.
						\param moduleId Defines the module id if the parameter is created for a module.
						\param modulePosition Defines the module position if the parameter is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameter(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueID, const std::string& dataTypeUniqueIDRef, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ParameterAccess access, bool createTemplate = false, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create a parameter with a referenced parameter template within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueId Specifies the unique id of the parameter.
						\param access Specifies the parameter access type.
						\param parameterTemplateUniqueIdRef Specifies the referenced template for the parameter.
						\param interfaceId Defines interface if parameter is created for a module.
						\param moduleId Defines the module id if the parameter is created for a module.
						\param modulePosition Defines the module position if the parameter is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameter(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ParameterAccess access, const std::string& parameterTemplateUniqueIdRef, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create a parameter with a normal datatype within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueId Specifies the unique id of the parameter.
						\param access Specifies the parameter access type.
						\param dataType Specifies the datatype for the parameter.
						\param createTemplate Create a parameter template not a parameter.
						\param interfaceId Defines interface if parameter is created for a module.
						\param moduleId Defines the module id if the parameter is created for a module.
						\param modulePosition Defines the module position if the parameter is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameter(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ParameterAccess access,
						        IndustrialNetwork::POWERLINK::Core::ObjectDictionary::IEC_Datatype dataType, bool createTemplate = false, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create a struct datatype within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueID Specifies the unique id of the struct datatype.
						\param name Specifies the name of the struct datatype.
						\param interfaceId Defines interface if struct datatype is created for a module.
						\param moduleId Defines the module id if the struct datatype is created for a module.
						\param modulePosition Defines the module position if the struct datatype is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateStructDatatype(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueID, const std::string& name, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create a var declaration within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param structUniqueId Specifies the struct that the var belongs to.
						\param uniqueId Specifies the unique id of the var declaration.
						\param name Specifies the name of the var declaration.
						\param datatype Specifies the datatype of the var declaration.
						\param size Specifies the size of the var declaration (only allowed datatype if BITSTRING)
						\param initialValue Specifies the initial value for the var declaration.
						\param interfaceId Defines interface if vardeclaration is created for a module.
						\param moduleId Defines the module id if the vardeclaration is created for a module.
						\param modulePosition Defines the module position if the vardeclaration is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateVarDeclaration(const std::string& networkId, const std::uint8_t nodeId, const std::string& structUniqueId, const std::string& uniqueId, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::IEC_Datatype datatype, std::uint32_t size = 1, const std::string& initialValue = "", const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create an array datatype within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueID Specifies the unique id of the array datatype.
						\param name Specifies the name of the array datatype.
						\param lowerLimit Specifies the lower limit of the array datatype.
						\param upperLimit Specifies the upper limit of the array datatype.
						\param dataType Specifies the datatype of the array datatype.
						\param interfaceId Defines interface if array datatype is created for a module.
						\param moduleId Defines the module id if the array datatype is created for a module.
						\param modulePosition Defines the module position if the array datatype is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateArrayDatatype(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueID, const std::string& name, std::uint32_t lowerLimit, std::uint32_t upperLimit, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::IEC_Datatype dataType, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create an enum datatype within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueID Specifies the unique id of the enum datatype.
						\param name Specifies the name of the enum datatype.
						\param dataType Specifies the datatype of the enum datatype.
						\param size Specifies the size of the enum datatype.
						\param interfaceId Defines interface if enum datatype is created for a module.
						\param moduleId Defines the module id if the enum datatype is created for a module.
						\param modulePosition Defines the module position if the enum datatype is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateEnumDatatype(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueID, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::IEC_Datatype dataType, std::uint32_t size = 0, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to create an enum value within a controlled nodes application process.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterUniqueId Specifies the unique id of the enum datatype.
						\param name Specifies the name of the enum value.
						\param value Specifies the enum value.
						\param interfaceId Defines interface if enum value is created for a module.
						\param moduleId Defines the module id if the enum value is created for a module.
						\param modulePosition Defines the module position if the enum value is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateEnumValue(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& name, const std::string& value, const std::string& interfaceId = "", const std::string& moduleId = "", std::uint32_t modulePosition = 0);
						//! Member to retrieve the datatype size of a complex datatype.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dataTypeUniqueId Specifies the unique id of the complex datatype.
						\param size Byte size of the datatype.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetDatatypeSize(const std::string& networkId, const std::uint8_t nodeId, const std::string& dataTypeUniqueId, std::uint32_t& size);
						//! Member to set the allowed values for a parameter.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterUniqueId Parameter to add the allowed values.
						\param allowedValues String vector including the allowed values.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetParameterAllowedValues(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, std::vector<std::string>& allowedValues);
						//! Member to set the default value of a parameter.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterUniqueId Parameter to set the default value.
						\param paramDefaultValue Default values string.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetParameterDefaultValue(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& paramDefaultValue);
						//! Member to set the actual value of a parameter.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterUniqueId Parameter to set the actual value.
						\param paramActualValue Actual values string.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetParameterActualValue(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& paramActualValue);
						//! Member to set the allowed range for a parameter.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterUniqueId Parameter to set the allowed range.
						\param minValue Minimum value of the range.
						\param maxValue Maximum value of the range.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetParameterAllowedRange(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterUniqueId, const std::string& minValue, const std::string& maxValue);
						//! Member to create a parameter group.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterGroupUniqueId Defines the parameter group unique id.
						\param interfaceId Defines interface if parameter group is created for a module.
						\param moduleId Defines the module id if the parameter group is created for a module.
						\param modulePosition Defines the module position if the parameter group is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameterGroup(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition);
						//! Member to create a child parameter group.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterGroupUniqueId Defines the parameter group unique id.
						\param parentParameterGroupUniqueId Defines the parent parameter group unique id.
						\param bitOffset Defines the bitoffset of the parameter group.
						\param interfaceId Defines interface if parameter is created for a module.
						\param moduleId Defines the module id if the parameter is created for a module.
						\param modulePosition Defines the module position if the parameter is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameterGroup(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& parentParameterGroupUniqueId, std::uint16_t bitOffset, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition);
						//! Member to create a child parameter group with conditional value.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterGroupUniqueId Defines the parameter group unique id.
						\param parentParameterGroupUniqueId Defines the parent parameter group unique id.
						\param conditionalUniqueId Defines the conditional parameter referenced by the parameter group.
						\param conditionalValue Defines the conditional value for the parameter group.
						\param bitOffset Defines the bitoffset of the parameter group.
						\param interfaceId Defines interface if parameter is created for a module.
						\param moduleId Defines the module id if the parameter is created for a module.
						\param modulePosition Defines the module position if the parameter is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameterGroup(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& parentParameterGroupUniqueId, const std::string& conditionalUniqueId, const std::string& conditionalValue, std::uint16_t bitOffset, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition);
						//! Member to create a parameter reference.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterGroupUniqueId Defines the parameter group unique id.
						\param parameterUniqueIdRef Defines the referenced parameter.
						\param actualValue Defines an optional actual value for the referenced parameter.
						\param bitOffset Defines the bitoffset of the parameter group.
						\param interfaceId Defines interface if parameter reference is created for a module.
						\param moduleId Defines the module id if the parameter reference is created for a module.
						\param modulePosition Defines the module position if the parameter reference is created for a module.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateParameterReference(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& parameterUniqueIdRef, const std::string& actualValue, std::uint16_t bitOffset, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition);
						//! Member to retrieve the offset for a mapped object within the managing node process image.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive).
						\param objectId Specifies the mapping object on the controlled node (0x16XX / 0x1AXX).
						\param subObjectId Specifies the mapping subobject on the controlled node.
						\param offset Bitoffset to return.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetMappingObjectProcessImageOffset(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, std::uint32_t objectId, std::uint32_t subObjectId, std::uint32_t& offset);
						//! Member to retrieve the offset for a mapped parameter within the managing node process image.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param dir Specifies the direction of the mapping (transmit or receive).
						\param paramName Specifies the parameter name.
						\param offset Bitoffset to return.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetMappingParameterProcessImageOffset(const std::string& networkId, const std::uint8_t nodeId, const IndustrialNetwork::POWERLINK::Core::ObjectDictionary::Direction& dir, const std::string& paramName, std::uint32_t& offset);
						//! Member to evaluate whether a parameter group condition is met.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param parameterGroupUniqueId Defines the parameter group unique id.
						\param interfaceId Defines interface if parameter reference is created for a module.
						\param moduleId Defines the module id if the parameter reference is created for a module.
						\param modulePosition Defines the module position if the parameter reference is created for a module.
						\param conditionMet Returns true if condition is met.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result EvaluateParameterGroupCondition(const std::string& networkId, const std::uint8_t nodeId, const std::string& parameterGroupUniqueId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, bool& conditionMet);

						/** @} */
						/** \addtogroup modularnode
						* @{
						*/
						//! Member to create a modular head node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param nodeName Name of the modular head node.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateModularHeadNode(const std::string& networkId, const std::uint8_t nodeId, const std::string& nodeName);
						//! Member to create an object for a module.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param modulePosition Defines the module position.
						\param objectId Specifies the object identifier.
						\param objectType Specifies the ObjectType of the object.
						\param name Specifies the name of the object.
						\param dataType Specifies the %POWERLINK datatype of the object.
						\param accessType Specifies the access type of the object.
						\param pdoMapping Specifies the PDOmapping attribute of the object.
						\param defaultValueToSet Specifies the default value of the object. (may be empty)
						\param actualValueToSet Specifies the actual value of the object. (may be empty)
						\param rangeSelector Defines the range selector for the object.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateModuleObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet, const std::string& rangeSelector);
						//! Member to create a parameter object for a module.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param modulePosition Defines the module position.
						\param objectId Specifies the object identifier.
						\param objectType Specifies the ObjectType of the object.
						\param name Specifies the name of the object.
						\param dataType Specifies the %POWERLINK datatype of the object.
						\param accessType Specifies the access type of the object.
						\param pdoMapping Specifies the PDOmapping attribute of the object.
						\param uniqueIdRef Parameter referenced by the object.
						\param defaultValueToSet Specifies the default value of the object. (may be empty)
						\param actualValueToSet Specifies the actual value of the object. (may be empty)
						\param rangeSelector Defines the range selector for the object.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateModuleParameterObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet, const std::string& rangeSelector);
						//! Member to create a module subobject.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param modulePosition Defines the module position.
						\param objectId Specifies the object identifier.
						\param subObjectId Specifies the sub object identifier.
						\param objectType Specifies the ObjectType of the object.
						\param name Specifies the name of the object.
						\param dataType Specifies the %POWERLINK datatype of the object.
						\param accessType Specifies the access type of the object.
						\param pdoMapping Specifies the PDOmapping attribute of the object.
						\param defaultValueToSet Specifies the default value of the object. (may be empty)
						\param actualValueToSet Specifies the actual value of the object. (may be empty)
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateModuleSubObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, std::uint16_t subObjectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& defaultValueToSet, const std::string& actualValueToSet);
						//! Member to create a parameter subobject for a module.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param modulePosition Defines the module position.
						\param objectId Specifies the object identifier.
						\param subObjectId Specifies the sub object identifier.
						\param objectType Specifies the ObjectType of the object.
						\param name Specifies the name of the object.
						\param dataType Specifies the %POWERLINK datatype of the object.
						\param accessType Specifies the access type of the object.
						\param pdoMapping Specifies the PDOmapping attribute of the object.
						\param uniqueIdRef Parameter referenced by the object.
						\param defaultValueToSet Specifies the default value of the object. (may be empty)
						\param actualValueToSet Specifies the actual value of the object. (may be empty)
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateModuleParameterSubObject(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t objectId, std::uint16_t subObjectId, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::ObjectType objectType, const std::string& name, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PlkDataType dataType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::AccessType accessType, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping, const std::string& uniqueIdRef, const std::string& defaultValueToSet, const std::string& actualValueToSet);
						//! Member to create an interface for a modular head node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param uniqueId Specifies the interface unique id.
						\param type Specifies the supported module type for the interface.
						\param moduleAddressing Specifies the module addressing scheme for the interface.
						\param maxModules Specifies the maximum number of supported modules of the interface.
						\param unusedSlots Defines whether unused slots are allowed on the interface.
						\param multipleModules Defines whether multiple modules are allowed in the interface.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateInterface(const std::string& networkId, const std::uint8_t nodeId, const std::string& uniqueId, const std::string& type, IndustrialNetwork::POWERLINK::Core::ModularNode::ModuleAddressing moduleAddressing, std::uint32_t maxModules, bool unusedSlots, bool multipleModules);
						//! Member to create module on the modular head node.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module id if the parameter.
						\param modulePosition Defines the module position if the parameter.
						\param moduleAddress Address of the module on the interface.
						\param moduleType Defines the subbus type of the module.
						\param moduleName Defines the module name.
						\param moduleAddressing Defines the module addressing scheme.
						\param minPosition Defines the minimum position of the module on the interface.
						\param maxPosition Defines the maximum position of the module on the interface.
						\param minAddress Defines the minimum address of the module on the interface.
						\param maxAddress Defines the maximum address of the module on the interface.
						\param maxCount Defines the maximum count for the modules on the interface.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, std::uint32_t moduleAddress, const std::string& moduleType, const std::string& moduleName, IndustrialNetwork::POWERLINK::Core::ModularNode::ModuleAddressing moduleAddressing, std::uint16_t minPosition, std::uint16_t maxPosition, std::uint16_t minAddress, std::uint16_t maxAddress, std::uint16_t maxCount);
						//! Member to create an index range for a modular head node interface.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param name Defines the name of the interface range.
						\param baseIndex Defines the base index for the interface range.
						\param maxIndex Defines the maximum index for the interface range.
						\param maxSubIndex Defines the maximum subindex for the interface range.
						\param sortStep Defines the sort step for the interface range.
						\param sortMode Defines the sort mode for the interface range.
						\param sortNumber Defines the sort number for the interface range.
						\param pdoMapping Defines an optional PDOMapping for the interface range.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result CreateRange(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& name, std::uint32_t baseIndex, std::uint32_t maxIndex, std::uint32_t maxSubIndex, std::uint32_t sortStep, IndustrialNetwork::POWERLINK::Core::ModularNode::SortMode sortMode, IndustrialNetwork::POWERLINK::Core::ModularNode::SortNumber sortNumber, IndustrialNetwork::POWERLINK::Core::ObjectDictionary::PDOMapping pdoMapping);
						//! Member to remove a module from the interface.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param modulePosition Defines the module position.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result RemoveModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition);
						//! Member to enable / disable a module.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param modulePosition Defines the module position.
						\param enable Enable / Disable flag.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result EnableModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t modulePosition, bool enable);
						//! Member to move a module on the interface.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param oldPosition Defines the old postition of the module on the interface.
						\param newPosition Defines the new postition of the module on the interface.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result MoveModule(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t oldPosition, std::uint32_t newPosition);
						//! Member to retrieve the current calculated index / subindex of a module object / subobject.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface unique id.
						\param moduleId Defines the module unique id.
						\param position Defines the module position.
						\param originalIndex Original index from the XDD.
						\param originalSubIndex Original subindex from the XDD.
						\param returnIndex Return value with the current valid index.
						\param returnSubIndex Return value with the current valid subindex.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetModuleObjectCurrentIndex(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t position, std::uint32_t originalIndex, std::int32_t originalSubIndex, std::uint32_t& returnIndex, std::int32_t& returnSubIndex);
						//! Member to retrieve the current parameter / parameter group / parameter reference / parameter template name for a module.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface if parameter.
						\param moduleId Defines the module id if the parameter.
						\param position Defines the module position if the parameter.
						\param originalParamName Original parameter name from the XDD.
						\param parameterName Return values with the current valid parameter name.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result GetModuleParameterCurrentName(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t position, const std::string& originalParamName, std::string& parameterName);
						//! Member to set the sub-bus address of a module during runtime.
						/*!
						\param networkId Specifies the identifier for the network.
						\param nodeId Specifies the node identifier.
						\param interfaceId Defines interface if parameter.
						\param moduleId Defines the module id if the parameter.
						\param position Defines the module position if the parameter.
						\param address Adress to be set.
						\return IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result
						*/
						IndustrialNetwork::POWERLINK::Core::ErrorHandling::Result SetModuleAddress(const std::string& networkId, const std::uint8_t nodeId, const std::string& interfaceId, const std::string& moduleId, std::uint32_t position, std::uint32_t address);
						/** @} */


					private:
						//singleton
						OpenConfiguratorCore();
						OpenConfiguratorCore(OpenConfiguratorCore const&);
						void operator=(OpenConfiguratorCore const&);
				};
			}
		}
	}
}
#endif
