#include "engine/gui/MyGUI.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace {
	sf::FloatRect offsetRect(const sf::FloatRect& rect, sf::Vector2f offset) {
		return sf::FloatRect(rect.position + offset, rect.size);
	}

	// SFML 3 exposes hardware scissor testing through sf::View. Keep the
	// current coordinate transform and only replace its pixel-write region.
	class ScopedScissor {
	public:
		ScopedScissor(sf::RenderTarget& target, const sf::FloatRect& worldClip) :
			target(target), previousView(target.getView()) {
			const sf::Vector2u targetSize = target.getSize();
			if (targetSize.x == 0 || targetSize.y == 0)
				return;

			const sf::Vector2f topLeft = worldClip.position;
			const sf::Vector2f bottomRight = worldClip.position + worldClip.size;
			const std::array<sf::Vector2f, 4> corners = {
				topLeft,
				sf::Vector2f(bottomRight.x, topLeft.y),
				bottomRight,
				sf::Vector2f(topLeft.x, bottomRight.y)
			};

			float minX = static_cast<float>(targetSize.x);
			float minY = static_cast<float>(targetSize.y);
			float maxX = 0.f;
			float maxY = 0.f;
			for (const sf::Vector2f corner : corners) {
				const sf::Vector2i pixel = target.mapCoordsToPixel(corner, previousView);
				minX = std::min(minX, static_cast<float>(pixel.x));
				minY = std::min(minY, static_cast<float>(pixel.y));
				maxX = std::max(maxX, static_cast<float>(pixel.x));
				maxY = std::max(maxY, static_cast<float>(pixel.y));
			}

			minX = std::clamp(std::floor(minX), 0.f, static_cast<float>(targetSize.x));
			minY = std::clamp(std::floor(minY), 0.f, static_cast<float>(targetSize.y));
			maxX = std::clamp(std::ceil(maxX), minX, static_cast<float>(targetSize.x));
			maxY = std::clamp(std::ceil(maxY), minY, static_cast<float>(targetSize.y));

			sf::View clippedView = previousView;
			clippedView.setScissor(sf::FloatRect(
				{minX / static_cast<float>(targetSize.x), minY / static_cast<float>(targetSize.y)},
				{(maxX - minX) / static_cast<float>(targetSize.x),
				 (maxY - minY) / static_cast<float>(targetSize.y)}));
			target.setView(clippedView);
			active = true;
		}

		~ScopedScissor() {
			if (active)
				target.setView(previousView);
		}

		ScopedScissor(const ScopedScissor&) = delete;
		ScopedScissor& operator=(const ScopedScissor&) = delete;

	private:
		sf::RenderTarget& target;
		sf::View previousView;
		bool active = false;
	};
}

namespace gui {
	void UIBase::draw(sf::RenderTarget& r, sf::Vector2f drawOffset, sf::FloatRect clipArea, UIwindowManager&) {
		if (!isShow)
			return;
		const sf::FloatRect absoluteRect = offsetRect(posRect, drawOffset);
		if (!absoluteRect.findIntersection(clipArea))
			return;
		_builtinGUIdraw::Rect(
			r,
			absoluteRect.position,
			absoluteRect.position + absoluteRect.size,
			styles[currentStatu].backgroundColor,
			styles[currentStatu].outlineColor,
			styles[currentStatu].outlineThickness
		);
	}

	void ImageObject::draw(sf::RenderTarget& r, sf::Vector2f drawOffset, sf::FloatRect clipArea, UIwindowManager& windowManager) {
		if (!isShow)
			return;
		const sf::FloatRect absoluteRect = offsetRect(posRect, drawOffset);
		if (!absoluteRect.findIntersection(clipArea))
			return;
		UIBase::draw(r, drawOffset, clipArea, windowManager);

		const sf::Vector2f imgSize = static_cast<sf::Vector2f>(imageManager[imageId].getSize());
		sf::Vector2f realScale = scale;
		if (realScale.x < 0) realScale.x = posRect.size.x / imgSize.x;
		if (realScale.y < 0) realScale.y = posRect.size.y / imgSize.y;
		sf::Sprite imageRender(imageManager[imageId]);
		imageRender.setPosition(absoluteRect.position + ((posRect.size - imgSize.componentWiseMul(realScale)) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(align)));
		imageRender.setScale(realScale);
		imageRender.setColor(imageColors[currentStatu]);
		r.draw(imageRender);
	}

	void TextObject::updateTextMetrics() {
		if (!isDirty(DirtyTextMetrics))
			return;

		// Keep the existing per-character measurement workaround. SFML's
		// local/global bounds do not match the positioning expected by this UI.
		textRender.setString("_");
		textRender.setCharacterSize(characterSize);
		textRender.setLineSpacing(lineSpacing);
		textRender.setLetterSpacing(letterSpacing);
		textRender.setPosition({0, 0});
		textRenderOffsetFix.x = textRender.getGlobalBounds().position.x;
		textRenderOffsetFix.y = textRender.getGlobalBounds().position.y + textRender.getGlobalBounds().size.y - characterSize;

		textRender.setString(text);
		characterPositions.clear();
		characterPositions.reserve(text.getSize() + 1);
		characterPositions.push_back({});

		// Cache the same caret positions as sf::Text::findCharacterPos(), but
		// calculate the complete string in one pass instead of restarting at
		// the beginning for every character.
		const sf::Font& textFont = textRender.getFont();
		const bool isBold = (textRender.getStyle() & sf::Text::Bold) != 0;
		float whitespaceWidth = textFont.getGlyph(U' ', characterSize, isBold).advance;
		const float actualLetterSpacing = (whitespaceWidth / 3.f) * (letterSpacing - 1.f);
		whitespaceWidth += actualLetterSpacing;
		const float actualLineSpacing = textFont.getLineSpacing(characterSize) * lineSpacing;
		sf::Vector2f characterPosition;
		std::uint32_t previousCharacter = 0;
		for (const char32_t currentCharacter : text) {
			characterPosition.x += textFont.getKerning(previousCharacter, currentCharacter, characterSize, isBold);
			previousCharacter = currentCharacter;

			switch (currentCharacter) {
			case U' ':
				characterPosition.x += whitespaceWidth;
				break;
			case U'\t':
				characterPosition.x += whitespaceWidth * 4.f;
				break;
			case U'\n':
				characterPosition.y += actualLineSpacing;
				characterPosition.x = 0;
				break;
			default:
				characterPosition.x += textFont.getGlyph(currentCharacter, characterSize, isBold).advance + actualLetterSpacing;
				break;
			}
			characterPositions.push_back(characterPosition);
		}

		textRect.size.x = 0;
		for (const sf::Vector2f& position : characterPositions) {
			if (position.x > textRect.size.x)
				textRect.size.x = position.x;
		}
		textRect.size.y = characterPositions.back().y + characterSize;
		clearDirty(DirtyTextMetrics);
	}

	void TextObject::draw(sf::RenderTarget& r, sf::Vector2f drawOffset, sf::FloatRect clipArea, UIwindowManager& windowManager) {
		if (!isShow)
			return;
		const sf::FloatRect absoluteRect = offsetRect(posRect, drawOffset);
		updateTextMetrics();
		textRender.setFillColor(textStyles[currentStatu].fillColor);
		textRender.setOutlineColor(textStyles[currentStatu].outlineColor);
		const sf::Vector2f alignedOffset = ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(align));
		textRect.position = posRect.position + alignedOffset;
		const sf::FloatRect absoluteTextRect(textRect.position + drawOffset, textRect.size);
		const bool rectVisible = absoluteRect.findIntersection(clipArea).has_value();
		const bool textVisible = absoluteTextRect.findIntersection(clipArea).has_value();
		if (!rectVisible && !textVisible)
			return;
		if (rectVisible)
			UIBase::draw(r, drawOffset, clipArea, windowManager);
		if (!textVisible)
			return;
		textRender.setPosition(absoluteRect.position - textRenderOffsetFix + alignedOffset);
		r.draw(textRender);
	}

	sf::Vector2f InputObject::updateTextLayout() {
		updateTextMetrics();
		textRender.setFillColor(textStyles[currentStatu].fillColor);
		textRender.setOutlineColor(textStyles[currentStatu].outlineColor);
		const sf::Vector2f alignedOffset = ((posRect.size - textRect.size) / 2.f).componentWiseMul(static_cast<sf::Vector2f>(align));
		textRender.setPosition(-textRenderOffsetFix + alignedOffset + scroll);
		textRect.position = alignedOffset;
		return getCharacterPosition(cursor) + textRender.getPosition() + textRenderOffsetFix;
	}

	void InputObject::ensureCursorVisible() {
		sf::Vector2f cursorPos = updateTextLayout();
		if (textRect.size.x > posRect.size.x) {
			if (cursorPos.x < 0)
				scroll.x += 0 - cursorPos.x;
			else if (cursorPos.x > posRect.size.x)
				scroll.x += posRect.size.x - cursorPos.x;
			if (textRect.position.x + scroll.x + textRect.size.x < posRect.size.x)
				scroll.x += posRect.size.x - (textRect.position.x + scroll.x + textRect.size.x);
			if (textRect.position.x + scroll.x > 0)
				scroll.x += 0 - (textRect.position.x + scroll.x);
		}
		else scroll.x = 0;
		if (textRect.size.y > posRect.size.y) {
			if (posRect.size.y < characterSize){
				scroll.y += 0 - cursorPos.y;
			}
			else {
				if (cursorPos.y < 0)
					scroll.y += 0 - cursorPos.y;
				else if (cursorPos.y + characterSize > posRect.size.y)
					scroll.y += posRect.size.y - (cursorPos.y + characterSize);
			}
			if (textRect.position.y + scroll.y + textRect.size.y < posRect.size.y)
				scroll.y += posRect.size.y - (textRect.position.y + scroll.y + textRect.size.y);
			if (textRect.position.y + scroll.y > 0)
				scroll.y += 0 - (textRect.position.y + scroll.y);
		}
		else scroll.y = 0;
	}

	void InputObject::draw(sf::RenderTarget& r, sf::Vector2f drawOffset, sf::FloatRect clipArea, UIwindowManager& windowManager) {
		if (!isShow)
			return;
		const sf::FloatRect absoluteRect = offsetRect(posRect, drawOffset);
		const auto inputClip = absoluteRect.findIntersection(clipArea);
		if (!inputClip)
			return;

		UIBase::draw(r, drawOffset, clipArea, windowManager);
		const sf::Vector2f cursorPos = updateTextLayout();
		const sf::Vector2f localTextPosition = textRender.getPosition();
		textRender.setPosition(absoluteRect.position + localTextPosition);
		{
			ScopedScissor scissor(r, *inputClip);
			r.draw(textRender);
			if (currentStatu == gui::UIBase::Focus && windowManager.cursorBlinkTick < windowManager.cursorBlinkRate / 2) {
				_builtinGUIdraw::Line(
					r,
					absoluteRect.position + cursorPos,
					absoluteRect.position + cursorPos + sf::Vector2f(0, static_cast<float>(characterSize)),
					textStyles[currentStatu].fillColor,
					2.0f
				);
			}
		}
		textRender.setPosition(localTextPosition);
	}

	void AreaObject::draw(sf::RenderTarget& r, sf::Vector2f drawOffset, sf::FloatRect clipArea, UIwindowManager& windowManager) {
		if (!isShow)
			return;
		const sf::FloatRect absoluteRect = offsetRect(posRect, drawOffset);
		const auto childClip = absoluteRect.findIntersection(clipArea);
		if (!childClip)
			return;

		UIBase::draw(r, drawOffset, clipArea, windowManager);
		ScopedScissor scissor(r, *childClip);
		const sf::Vector2f childDrawOffset = absoluteRect.position + scroll;
		for (auto& elem : sub.riterate())
			elem->draw(r, childDrawOffset, *childClip, windowManager);
	}

	void AreaObject::updateScroll(UIwindowManager& windowManager) {
		if (scrollVelocity != sf::Vector2f()) {
			if (scrollVelocity.lengthSquared() < windowManager.scrollResistance * windowManager.scrollResistance) {
				scrollVelocity = sf::Vector2f();
			}
			else {
				scrollVelocity -= scrollVelocity
					.componentWiseDiv(sf::Vector2f(scrollVelocity.length(), scrollVelocity.length()))
					.componentWiseMul(sf::Vector2f(windowManager.scrollResistance, windowManager.scrollResistance));
			}
			scroll += scrollVelocity;
		}
		if (mouseDragScrollable != sf::Vector2i() || mouseWheelScrollable != sf::Vector2i())
			ensureScrollLimit();
		for (auto& elem : sub.iterate<AreaObject>())
			elem.updateScroll(windowManager);
	}
}
