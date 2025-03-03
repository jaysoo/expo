/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ABI43_0_0React/ABI43_0_0renderer/components/progressbar/AndroidProgressBarMeasurementsManager.h>
#include <ABI43_0_0React/ABI43_0_0renderer/components/rncore/EventEmitters.h>
#include <ABI43_0_0React/ABI43_0_0renderer/components/rncore/Props.h>
#include <ABI43_0_0React/ABI43_0_0renderer/components/view/ConcreteViewShadowNode.h>

namespace ABI43_0_0facebook {
namespace ABI43_0_0React {

extern const char AndroidProgressBarComponentName[];

/*
 * `ShadowNode` for <AndroidProgressBar> component.
 */
class AndroidProgressBarShadowNode final : public ConcreteViewShadowNode<
                                               AndroidProgressBarComponentName,
                                               AndroidProgressBarProps,
                                               AndroidProgressBarEventEmitter> {
 public:
  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  // Associates a shared `AndroidProgressBarMeasurementsManager` with the node.
  void setAndroidProgressBarMeasurementsManager(
      const std::shared_ptr<AndroidProgressBarMeasurementsManager>
          &measurementsManager);

#pragma mark - LayoutableShadowNode

  Size measureContent(
      LayoutContext const &layoutContext,
      LayoutConstraints const &layoutConstraints) const override;

 private:
  std::shared_ptr<AndroidProgressBarMeasurementsManager> measurementsManager_;
};

} // namespace ABI43_0_0React
} // namespace ABI43_0_0facebook
