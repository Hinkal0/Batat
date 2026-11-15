#include "LabeledValue.h"
#include <utility>
#include "Text.h"

static constexpr float GAP = 5.0f;

LabeledValue::LabeledValue(sf::Vector2f rightEdge, std::string label, std::string value,
    sf::Color labelColor, sf::Color valueColor)
  : UIElement(rightEdge), label(std::move(label)), labelColor(labelColor), valueColor(valueColor),
    value(std::move(value)) {}

void LabeledValue::drawContents(sf::RenderTarget& target, sf::RenderStates states) const {
  Text::draw(target, value, {}, 1, valueColor, Text::Align::Right, true, states);
  Text::draw(target, label, {-Text::width(value) - GAP, 0.0f},
    1, labelColor, Text::Align::Right, true, states);
}
