/* Copyright 2013-2019 Homegear GmbH
 *
 * Homegear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Homegear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Homegear.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include "UiBase.h"

namespace Ui {

UiBase::UiBase(const std::string &path, const std::string &type, const std::atomic_bool *frontendConnected) : Flows::INode(path, type, frontendConnected) {
}

UiBase::~UiBase() = default;

bool UiBase::init(const Flows::PNodeInfo &info) {
  try {
    auto settingsIterator = info->info->structValue->find("inputIndexes");
    if (settingsIterator != info->info->structValue->end()) {
      _variableInputIndexByNodeInputIndex.reserve(settingsIterator->second->arrayValue->size());
      uint32_t nodeInputIndex = 0;
      for (auto &element : *settingsIterator->second->arrayValue) {
        if (element->arrayValue->size() != 2) continue;
        _variableInputIndexByNodeInputIndex.emplace_back(std::make_pair(element->arrayValue->at(0)->integerValue, element->arrayValue->at(1)->integerValue));
        _nodeInputIndexByVariableInputIndex[element->arrayValue->at(0)->integerValue][element->arrayValue->at(1)->integerValue] = nodeInputIndex++;
      }
    }

    settingsIterator = info->info->structValue->find("outputIndexes");
    if (settingsIterator != info->info->structValue->end()) {
      _variableOutputIndexByNodeOutputIndex.reserve(settingsIterator->second->arrayValue->size());
      uint32_t nodeOutputIndex = 0;
      for (auto &element : *settingsIterator->second->arrayValue) {
        if (element->arrayValue->size() != 2) continue;
        _variableOutputIndexByNodeOutputIndex.emplace_back(std::make_pair(element->arrayValue->at(0)->integerValue, element->arrayValue->at(1)->integerValue));
        _nodeOutputIndexByVariableOutputIndex[element->arrayValue->at(0)->integerValue][element->arrayValue->at(1)->integerValue] = nodeOutputIndex++;
      }
    }

    settingsIterator = info->info->structValue->find("unit");
    if (settingsIterator != info->info->structValue->end()) _unit = settingsIterator->second->stringValue;

    settingsIterator = info->info->structValue->find("inputRendering");
    if (settingsIterator != info->info->structValue->end()) _inputRendering = settingsIterator->second;

    settingsIterator = info->info->structValue->find("dynamicMetadata");
    if (settingsIterator != info->info->structValue->end()) _dynamicMetadata = settingsIterator->second;

    settingsIterator = info->info->structValue->find("uielement");
    if (settingsIterator != info->info->structValue->end()) _uiElement = settingsIterator->second->stringValue;

    settingsIterator = info->info->structValue->find("room");
    if (settingsIterator != info->info->structValue->end()) _room = Flows::Math::getUnsignedNumber64(settingsIterator->second->stringValue);

    settingsIterator = info->info->structValue->find("uielementicon");
    if (settingsIterator != info->info->structValue->end()) _icon = settingsIterator->second->stringValue;

    settingsIterator = info->info->structValue->find("label");
    if (settingsIterator != info->info->structValue->end()) _label = settingsIterator->second->stringValue;

    settingsIterator = info->info->structValue->find("minimumvalue");
    if (settingsIterator != info->info->structValue->end()) {
      _rangeValuesSet = true;
      _minimumValue = Flows::Math::getDouble(settingsIterator->second->stringValue);
    }

    settingsIterator = info->info->structValue->find("maximumvalue");
    if (settingsIterator != info->info->structValue->end()) _maximumValue = Flows::Math::getDouble(settingsIterator->second->stringValue);

    settingsIterator = info->info->structValue->find("passthrough-input");
    if (settingsIterator != info->info->structValue->end()) _passThroughInput = settingsIterator->second->booleanValue;

    settingsIterator = info->info->structValue->find("roles");
    if (settingsIterator != info->info->structValue->end()) _roles = settingsIterator->second->booleanValue;

    settingsIterator = info->info->structValue->find("prefix");
    if (settingsIterator != info->info->structValue->end()) _prefix = settingsIterator->second->stringValue;

    settingsIterator = info->info->structValue->find("postfix");
    if (settingsIterator != info->info->structValue->end()) _postfix = settingsIterator->second->stringValue;

    settingsIterator = info->info->structValue->find("decimals");
    if (settingsIterator != info->info->structValue->end()) {
      _decimalPlaces = Flows::Math::getNumber(settingsIterator->second->stringValue);
      if (_decimalPlaces < 0) _decimalPlaces = -1;
      else if (_decimalPlaces > 100) _decimalPlaces = 100;
    }

    uint32_t outputs = 0;
    settingsIterator = info->info->structValue->find("outputs");
    if (settingsIterator != info->info->structValue->end()) outputs = settingsIterator->second->integerValue64;

    if (_uiElement.empty() || _room == 0 || _label.empty()) {
      _out->printError("Error in init: Not all required settings are configured.");
      return false;
    }

    for (uint32_t i = 0; i < outputs; i++) {
      subscribePeer(0x50000001, i, _id);
    }

    return true;
  }
  catch (const std::exception &ex) {
    _out->printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
  return false;
}

bool UiBase::start() {
  try {
    auto uiElementId = (uint64_t)getNodeData("uiElementId")->integerValue64;
    auto recreate = getNodeData("recreate");

    bool uiElementExists = false;

    if (uiElementId != 0) {
      auto parameters = std::make_shared<Flows::Array>();
      parameters->emplace_back(std::make_shared<Flows::Variable>(uiElementId));
      uiElementExists = invoke("uiElementExists", parameters)->booleanValue;
    }

    if (uiElementExists && recreate->type == Flows::VariableType::tBoolean && !recreate->booleanValue) {
      if (_roles) {
        auto roles = getNodeData("roles");
        if (!roles->structValue->empty()) {
          auto parameters = std::make_shared<Flows::Array>();
          parameters->reserve(2);
          parameters->emplace_back(std::make_shared<Flows::Variable>(_id));
          parameters->emplace_back(roles);
          auto result = invoke("registerUiNodeRoles", parameters);
          if (result->errorStruct) {
            _out->printWarning("Warning: Could not register roles.");
          }
        }
      }

      return true;
    }

    _out->printInfo("Info: Creating UI element.");

    {
      auto parameters = std::make_shared<Flows::Array>();
      parameters->emplace_back(std::make_shared<Flows::Variable>(_id));
      invoke("removeNodeUiElements", parameters);
    }

    Flows::PVariable uiElementTemplate;
    {
      auto parameters = std::make_shared<Flows::Array>();
      parameters->reserve(2);
      parameters->emplace_back(std::make_shared<Flows::Variable>(_uiElement));
      parameters->emplace_back(std::make_shared<Flows::Variable>("en-US"));
      uiElementTemplate = invoke("getUiElementTemplate", parameters);
    }

    if (uiElementTemplate->errorStruct) {
      _out->printError("Error: Could not (re-)create UI element. An error occured trying to get the UI element's template: " + uiElementTemplate->structValue->at("faultString")->stringValue);
      return false;
    }

    auto roles = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);

    auto typeIterator = uiElementTemplate->structValue->find("type");
    if (typeIterator == uiElementTemplate->structValue->end()) return false;

    auto inputPeers = std::make_shared<Flows::Variable>(Flows::VariableType::tArray);
    auto outputPeers = std::make_shared<Flows::Variable>(Flows::VariableType::tArray);
    if (typeIterator->second->stringValue == "simple") {
      auto templateIterator = uiElementTemplate->structValue->find("variableInputs");
      if (templateIterator != uiElementTemplate->structValue->end()) {
        auto outerArray = std::make_shared<Flows::Variable>(Flows::VariableType::tArray);
        outerArray->arrayValue->reserve(templateIterator->second->arrayValue->size());
        for (uint32_t i = 0; i < templateIterator->second->arrayValue->size(); i++) {
          auto inputIndexIterator = _nodeInputIndexByVariableInputIndex[0].find(i);
          if (inputIndexIterator == _nodeInputIndexByVariableInputIndex[0].end()) continue;
          auto nodeInputIndex = inputIndexIterator->second;
          auto entry = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
          entry->structValue->emplace("peer", std::make_shared<Flows::Variable>(0x50000000));
          entry->structValue->emplace("channel", std::make_shared<Flows::Variable>(nodeInputIndex));
          entry->structValue->emplace("name", std::make_shared<Flows::Variable>(_id));
          if (!_unit.empty()) entry->structValue->emplace("unit", std::make_shared<Flows::Variable>(_unit));
          if (_rangeValuesSet) {
            entry->structValue->emplace("minimum", std::make_shared<Flows::Variable>(_minimumValue));
            entry->structValue->emplace("minimumScaled", std::make_shared<Flows::Variable>(_minimumValue));
            entry->structValue->emplace("maximum", std::make_shared<Flows::Variable>(_maximumValue));
            entry->structValue->emplace("maximumScaled", std::make_shared<Flows::Variable>(_maximumValue));
          }
          if (_inputRendering && nodeInputIndex < _inputRendering->arrayValue->size()) entry->structValue->emplace("rendering", _inputRendering->arrayValue->at(nodeInputIndex));
          auto propertiesIterator = templateIterator->second->arrayValue->at(i)->structValue->find("properties");
          if (propertiesIterator != templateIterator->second->arrayValue->at(i)->structValue->end()) {
            auto roleIterator = propertiesIterator->second->structValue->find("role");
            if (roleIterator != propertiesIterator->second->structValue->end()) {
              roles->structValue->emplace("i" + std::to_string(nodeInputIndex), roleIterator->second);
            }
          }
          outerArray->arrayValue->emplace_back(entry);
        }
        inputPeers->arrayValue->emplace_back(outerArray);
      }

      templateIterator = uiElementTemplate->structValue->find("variableOutputs");
      if (templateIterator != uiElementTemplate->structValue->end()) {
        auto outerArray = std::make_shared<Flows::Variable>(Flows::VariableType::tArray);
        outerArray->arrayValue->reserve(templateIterator->second->arrayValue->size());
        for (uint32_t i = 0; i < templateIterator->second->arrayValue->size(); i++) {
          auto outputIndexIterator = _nodeOutputIndexByVariableOutputIndex[0].find(i);
          if (outputIndexIterator == _nodeOutputIndexByVariableOutputIndex[0].end()) continue;
          auto nodeOutputIndex = outputIndexIterator->second;
          auto entry = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
          entry->structValue->emplace("peer", std::make_shared<Flows::Variable>(0x50000001));
          entry->structValue->emplace("channel", std::make_shared<Flows::Variable>(nodeOutputIndex));
          entry->structValue->emplace("name", std::make_shared<Flows::Variable>(_id));
          auto propertiesIterator = templateIterator->second->arrayValue->at(i)->structValue->find("properties");
          if (propertiesIterator != templateIterator->second->arrayValue->at(i)->structValue->end()) {
            auto roleIterator = propertiesIterator->second->structValue->find("role");
            if (roleIterator != propertiesIterator->second->structValue->end()) {
              roles->structValue->emplace("o" + std::to_string(nodeOutputIndex), roleIterator->second);
            }
          }
          outerArray->arrayValue->emplace_back(entry);
        }
        outputPeers->arrayValue->emplace_back(outerArray);
      }
    } else {
      auto controlsIterator = uiElementTemplate->structValue->find("controls");
      if (controlsIterator != uiElementTemplate->structValue->end()) {
        inputPeers->arrayValue->reserve(controlsIterator->second->arrayValue->size());
        outputPeers->arrayValue->reserve(controlsIterator->second->arrayValue->size());
        uint32_t controlIndex = 0;
        for (auto &control : *controlsIterator->second->arrayValue) {
          auto templateIterator = control->structValue->find("variableInputs");
          if (templateIterator != control->structValue->end()) {
            auto outerArray = std::make_shared<Flows::Variable>(Flows::VariableType::tArray);
            outerArray->arrayValue->reserve(templateIterator->second->arrayValue->size());
            for (uint32_t i = 0; i < templateIterator->second->arrayValue->size(); i++) {
              auto inputIndexIterator = _nodeInputIndexByVariableInputIndex[controlIndex].find(i);
              if (inputIndexIterator == _nodeInputIndexByVariableInputIndex[controlIndex].end()) continue;
              auto nodeInputIndex = inputIndexIterator->second;
              auto entry = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
              entry->structValue->emplace("peer", std::make_shared<Flows::Variable>(0x50000000));
              entry->structValue->emplace("channel", std::make_shared<Flows::Variable>(nodeInputIndex));
              entry->structValue->emplace("name", std::make_shared<Flows::Variable>(_id));
              if (!_unit.empty()) entry->structValue->emplace("unit", std::make_shared<Flows::Variable>(_unit));
              if (_rangeValuesSet) {
                entry->structValue->emplace("minimum", std::make_shared<Flows::Variable>(_minimumValue));
                entry->structValue->emplace("minimumScaled", std::make_shared<Flows::Variable>(_minimumValue));
                entry->structValue->emplace("maximum", std::make_shared<Flows::Variable>(_maximumValue));
                entry->structValue->emplace("maximumScaled", std::make_shared<Flows::Variable>(_maximumValue));
              }
              if (_inputRendering && nodeInputIndex < _inputRendering->arrayValue->size()) entry->structValue->emplace("rendering", _inputRendering->arrayValue->at(nodeInputIndex));
              auto propertiesIterator = templateIterator->second->arrayValue->at(i)->structValue->find("properties");
              if (propertiesIterator != templateIterator->second->arrayValue->at(i)->structValue->end()) {
                auto roleIterator = propertiesIterator->second->structValue->find("role");
                if (roleIterator != propertiesIterator->second->structValue->end()) {
                  roles->structValue->emplace("i" + std::to_string(nodeInputIndex), roleIterator->second);
                }
              }
              outerArray->arrayValue->emplace_back(entry);
            }
            inputPeers->arrayValue->emplace_back(outerArray);
          }

          templateIterator = control->structValue->find("variableOutputs");
          if (templateIterator != control->structValue->end()) {
            auto outerArray = std::make_shared<Flows::Variable>(Flows::VariableType::tArray);
            outerArray->arrayValue->reserve(templateIterator->second->arrayValue->size());
            for (uint32_t i = 0; i < templateIterator->second->arrayValue->size(); i++) {
              auto outputIndexIterator = _nodeOutputIndexByVariableOutputIndex[controlIndex].find(i);
              if (outputIndexIterator == _nodeOutputIndexByVariableOutputIndex[controlIndex].end()) continue;
              auto nodeOutputIndex = outputIndexIterator->second;
              auto entry = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
              entry->structValue->emplace("peer", std::make_shared<Flows::Variable>(0x50000001));
              entry->structValue->emplace("channel", std::make_shared<Flows::Variable>(nodeOutputIndex));
              entry->structValue->emplace("name", std::make_shared<Flows::Variable>(_id));
              auto propertiesIterator = templateIterator->second->arrayValue->at(i)->structValue->find("properties");
              if (propertiesIterator != templateIterator->second->arrayValue->at(i)->structValue->end()) {
                auto roleIterator = propertiesIterator->second->structValue->find("role");
                if (roleIterator != propertiesIterator->second->structValue->end()) {
                  roles->structValue->emplace("o" + std::to_string(nodeOutputIndex), roleIterator->second);
                }
              }
              outerArray->arrayValue->emplace_back(entry);
            }
            outputPeers->arrayValue->emplace_back(outerArray);
          }
          controlIndex++;
        }
      }
    }

    auto data = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
    data->structValue->emplace("node", std::make_shared<Flows::Variable>(_id));
    data->structValue->emplace("room", std::make_shared<Flows::Variable>(_room));
    data->structValue->emplace("label", std::make_shared<Flows::Variable>(_label));
    if (!_icon.empty()) {
      auto iconStruct = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
      auto iconElement = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
      iconElement->structValue->emplace("name", std::make_shared<Flows::Variable>(_icon));
      iconElement->structValue->emplace("color", std::make_shared<Flows::Variable>("inactive"));
      iconStruct->structValue->emplace("main", iconElement);
      data->structValue->emplace("icons", iconStruct);
    }
    data->structValue->emplace("metadata", std::make_shared<Flows::Variable>(Flows::VariableType::tStruct));
    data->structValue->emplace("inputPeers", inputPeers);
    data->structValue->emplace("outputPeers", outputPeers);

    auto parameters = std::make_shared<Flows::Array>();
    parameters->reserve(3);
    parameters->emplace_back(std::make_shared<Flows::Variable>(_uiElement));
    parameters->emplace_back(data);
    if (!_dynamicMetadata) _dynamicMetadata = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
    parameters->emplace_back(_dynamicMetadata);
    auto result = invoke("addUiElement", parameters);
    if (result->errorStruct) {
      _out->printError("Error: Could not (re-)create UI element. An error occured trying to add the UI element: " + uiElementTemplate->structValue->at("faultString")->stringValue);
      return false;
    }

    setNodeData("uiElementId", result);
    if (_roles && !roles->structValue->empty()) {
      setNodeData("roles", roles);
      auto parameters2 = std::make_shared<Flows::Array>();
      parameters2->reserve(2);
      parameters2->emplace_back(std::make_shared<Flows::Variable>(_id));
      parameters2->emplace_back(roles);
      auto result2 = invoke("registerUiNodeRoles", parameters2);
      if (result2->errorStruct) {
        _out->printWarning("Warning: Could not register roles.");
      }
    }
    setNodeData("recreate", std::make_shared<Flows::Variable>(false));
    _created = recreate->type != Flows::VariableType::tBoolean;
    _recreated = true;

    return true;
  } catch (const std::exception &ex) {
    _out->printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
  return false;
}

void UiBase::setNodeVariable(const std::string &variable, const Flows::PVariable &value) {
  try {
    if (variable == "recreate") {
      setNodeData("recreate", std::make_shared<Flows::Variable>(true));
    }
  }
  catch (const std::exception &ex) {
    _out->printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void UiBase::variableEvent(const std::string &source, uint64_t peerId, int32_t channel, const std::string &variable, const Flows::PVariable &value, const Flows::PVariable &metadata) {
  try {
    Flows::PVariable outputMessage = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
    outputMessage->structValue->emplace("payload", value);
    output(channel, outputMessage);

    if (channel >= (int32_t)_variableOutputIndexByNodeOutputIndex.size()) return;
    auto indexIterator1 = _nodeInputIndexByVariableInputIndex.find(_variableOutputIndexByNodeOutputIndex.at(channel).first);
    if (indexIterator1 == _nodeInputIndexByVariableInputIndex.end()) return;
    auto indexIterator2 = indexIterator1->second.find(_variableOutputIndexByNodeOutputIndex.at(channel).second);
    if (indexIterator2 == indexIterator1->second.end()) return;

    auto inputIndex = indexIterator2->second;

    auto parameters = std::make_shared<Flows::Array>();
    parameters->reserve(5);
    parameters->emplace_back(std::make_shared<Flows::Variable>(source));
    parameters->emplace_back(std::make_shared<Flows::Variable>(0x50000000));
    parameters->emplace_back(std::make_shared<Flows::Variable>(inputIndex));
    parameters->emplace_back(std::make_shared<Flows::Variable>(_id));
    parameters->emplace_back(value);
    invoke("nodeBlueVariableEvent", parameters);

    setNodeData("i" + std::to_string(inputIndex), value);
    setNodeData("o" + std::to_string(channel), value);
  }
  catch (const std::exception &ex) {
    _out->printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void UiBase::input(const Flows::PNodeInfo &info, uint32_t index, const Flows::PVariable &message) {
  try {
    if (index >= _variableInputIndexByNodeInputIndex.size()) return;

    auto value = std::make_shared<Flows::Variable>();
    *value = *message->structValue->at("payload");

    if (_decimalPlaces >= 0 && value->type == Flows::VariableType::tFloat) {
      std::ostringstream out;
      out.precision(_decimalPlaces);
      out.imbue(std::locale(""));
      out << std::fixed << value->floatValue;
      value->stringValue = out.str();
      value->type = Flows::VariableType::tString;
    }

    if (!_prefix.empty() || !_postfix.empty()) {
      value->stringValue = _prefix + value->toString() + _postfix;
      value->type = Flows::VariableType::tString;
    }

    auto parameters = std::make_shared<Flows::Array>();
    parameters->reserve(5);
    parameters->emplace_back(std::make_shared<Flows::Variable>("nodeBlue"));
    parameters->emplace_back(std::make_shared<Flows::Variable>(0x50000000));
    parameters->emplace_back(std::make_shared<Flows::Variable>(index));
    parameters->emplace_back(std::make_shared<Flows::Variable>(_id));
    parameters->emplace_back(value);
    invoke("nodeBlueVariableEvent", parameters);

    setNodeData("i" + std::to_string(index), value);

    if (_passThroughInput) {
      if (index >= _variableInputIndexByNodeInputIndex.size()) return;
      auto indexIterator1 = _nodeOutputIndexByVariableOutputIndex.find(_variableInputIndexByNodeInputIndex.at(index).first);
      if (indexIterator1 == _nodeOutputIndexByVariableOutputIndex.end()) return;
      auto indexIterator2 = indexIterator1->second.find(_variableInputIndexByNodeInputIndex.at(index).second);
      if (indexIterator2 == indexIterator1->second.end()) return;
      auto outputIndex = indexIterator2->second;

      output(outputIndex, message);

      setNodeData("o" + std::to_string(outputIndex), message->structValue->at("payload"));
    }
  }
  catch (const std::exception &ex) {
    _out->printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

}