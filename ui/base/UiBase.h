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

#ifndef HOMEGEAR_NODES_UI_UI_BASE_UIBASE_H_
#define HOMEGEAR_NODES_UI_UI_BASE_UIBASE_H_

#include <homegear-node/INode.h>

#include <unordered_map>

namespace Ui {

class UiBase : public Flows::INode {
 public:
  UiBase(const std::string &path, const std::string &type, const std::atomic_bool *frontendConnected);
  ~UiBase() override;

  bool init(const Flows::PNodeInfo &info) override;
  bool start() override;

  void setNodeVariable(const std::string &variable, const Flows::PVariable &value) override;
 protected:
  bool _created = false;
  bool _recreated = false;
  std::vector<std::pair<uint32_t, uint32_t>> _variableInputIndexByNodeInputIndex;
  std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>> _nodeInputIndexByVariableInputIndex;
  std::vector<std::pair<uint32_t, uint32_t>> _variableOutputIndexByNodeOutputIndex;
  std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>> _nodeOutputIndexByVariableOutputIndex;
  Flows::PVariable _inputRendering;
  Flows::PVariable _dynamicMetadata;
  std::string _uiElement;
  uint64_t _room = 0;
  std::string _unit;
  std::string _icon;
  std::string _label;
  bool _rangeValuesSet = false;
  double _minimumValue = 0;
  double _maximumValue = 0;
  bool _passThroughInput = false;
  bool _roles = false;
  std::string _prefix;
  std::string _postfix;
  int32_t _decimalPlaces = -1;

  void variableEvent(const std::string &source, uint64_t peerId, int32_t channel, const std::string &variable, const Flows::PVariable &value, const Flows::PVariable &metadata) override;

  void input(const Flows::PNodeInfo &info, uint32_t index, const Flows::PVariable &message) override;
};

}

#endif //HOMEGEAR_NODES_UI_UI_BASE_UIBASE_H_
