#ifndef __SCYTHE_WIDGET_H__
#define __SCYTHE_WIDGET_H__

#include "renderable.h"

#include "common/types.h"
#include "math/vector2.h"

#include <list>

namespace scythe {

	enum class Flags : unsigned int {
		kRenderNever	= 0,
		kRenderAlways	= 1,
		kRenderIfActive	= 2,
		kSelectable		= 4,
		kDraggable		= 8,
		kDroppable		= 16
	};

	//! Base class for any widget
	/*!
	 We won't use any rotation, so we dont need to store transformation matrix.
	 But anyway i gonna think about that.
	*/
	class Widget : public virtual IRenderable {
	public:
		Widget();
		Widget(float x, float y, unsigned int flags);
		virtual ~Widget();

		void set_position(const Vector2& position);
		void set_active(bool active);
		void set_enabled(bool enabled);

		bool active() const;
		bool enabled() const;

		void AttachWidget(Widget * widget);
		void DetachWidget(Widget * widget);

		virtual void Update(float sec); //!< update this widget only
		void UpdateAll(float sec); //!< update entire tree starting from this widget
		virtual void Render() override;
		void RenderAll();
		
		void ActivateAll();
		void DeactivateAll();

		void Select(float x, float y); //!< activates only this if has selectable flag
		void SelectAll(float x, float y); //!< activates all selectable widgets
		
		bool HasFlag(Flags flag);

		bool BeginChildSearch();
		void EndChildSearch();
		bool GetNextChild(Widget *& child);
		
		void ObtainGlobalPosition(Vector2 * position);

	protected:

		Widget * parent_;
		std::list<Widget*> nodes_; //!< child nodes
		typename std::list<Widget*>::iterator child_iterator_; //!< for node enumeration
		Vector2 position_;
		unsigned int flags_;
		bool active_;
		bool enabled_;

	private:
		// Do we need to copy it?
		Widget(const Widget&) = delete;
		Widget& operator =(const Widget&) = delete;
	};

} // namespace scythe

#endif