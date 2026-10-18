<!-- https://www.w3.org/TR/SVG11/interact.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](filters.md) ⋅ [Next](linking.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 16 Interactivity

## <a id="Introduction"></a>16.1 Introduction

SVG content can be interactive (i.e., responsive to user-initiated events) by utilizing the following features in the SVG language:

- User-initiated actions such as button presses on the pointing device (e.g., a mouse) can cause [animations](animate.md) or [scripts](https://www.w3.org/TR/SVG11/script.html) to execute.
- The user can initiate hyperlinks to new Web pages (see [Links out of SVG content: the ‘a’ element](linking.md#Links)) by actions such as mouse clicks when the pointing device is positioned over particular graphics elements.
- In many cases, depending on the value of the [‘zoomAndPan’](#ZoomAndPanAttribute) attribute on the [‘svg’](struct.md#SVGElement) element and on the characteristics of the user agent, users are able to zoom into and pan around SVG content.
- User movements of the pointing device can cause changes to the [cursor](#Cursors) that shows the current position of the pointing device.

This chapter describes:

- information about [events](#SVGEvents), including under which circumstances events are triggered
- how to indicate whether a given document can be [zoomed and panned](#EnableZoomAndPanControls)
- how to specify which [cursors](#Cursors) to use

Related information can be found in other chapters:

- hyperlinks are discussed in [Links](linking.md)
- scripting and event attributes are discussed in [Scripting](https://www.w3.org/TR/SVG11/script.html)
- SVG's relationship to DOM2 events is discussed in [Relationship with DOM2 event model](https://www.w3.org/TR/SVG11/svgdom.html#RelationshipWithDOM2Events)
- animation is discussed in [Animation](animate.md)

## <a id="SVGEvents"></a>16.2 Complete list of supported events

The following aspects of SVG are affected by events:

- Using [SVG Document Object Model (DOM)](https://www.w3.org/TR/SVG11/svgdom.html), a script can register DOM 2 event listeners ([[DOM2EVENTS](refs.md#ref-DOM2EVENTS)], section 1.3) so that script can be invoked when a given event occurs.
- SVG includes [event attributes](https://www.w3.org/TR/SVG11/script.html#EventAttributes) on selected elements which define script that can be executed when a given event occurs in association with the given element.
- SVG's [animation elements](intro.md#TermAnimationElement) can be defined to begin or end based on events.

The following table lists all of the events which are recognized and supported in SVG. The *Event name* in the first column is the name to use within SVG's [animation elements](intro.md#TermAnimationElement) to define the events which can start or end animations. The *DOM2 name* in the second column is the name to use when defining DOM 2 event listeners ([[DOM2EVENTS](refs.md#ref-DOM2EVENTS)], section 1.3). The *Event attribute name* in the fourth column contains the corresponding name of the [event attributes](https://www.w3.org/TR/SVG11/script.html#EventAttributes) that can be attached to elements in the SVG language.

Requirements in the table on whether an event of a given type bubbles or is cancelable apply only to events that are created and dispatched by the user agent. Events of those types created from script using the createEvent method on the DocumentEvent interface can be made to bubble or be cancelable with the initEvent method.

| Event name and description | DOM2 name | DOM2 category | Event attribute name |
| --- | --- | --- | --- |
| <a id="FocusInEvent"></a>**focusin** Occurs when an element receives focus, such as when a [‘text’](text.md#TextElement) becomes selected. | DOMFocusIn | UIEvent | [onfocusin](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute) |
| <a id="FocusOutEvent"></a>**focusout** Occurs when an element loses focus, such as when a [‘text’](text.md#TextElement) becomes unselected. | DOMFocusOut | UIEvent | [onfocusout](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute) |
| <a id="ActivateEvent"></a>**activate** Occurs when an element is activated, for instance, through a mouse click or a keypress. A numerical argument is provided to give an indication of the type of activation that occurs: 1 for a simple activation (e.g. a simple click or Enter), 2 for hyperactivation (for instance a double click or Shift Enter). | DOMActivate | UIEvent | [onactivate](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute) |
| <a id="ClickEvent"></a>**click** Occurs when the pointing device button is clicked over an element. A click is defined as a mousedown and mouseup over the same screen location. The sequence of these events is: `mousedown`, `mouseup`, `click`. If multiple clicks occur at the same screen location, the sequence repeats with the `detail` attribute incrementing with each repetition. | (same) | MouseEvent | [onclick](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute) |
| <a id="MouseDownEvent"></a>**mousedown** Occurs when the pointing device button is pressed over an element. | (same) | MouseEvent | [onmousedown](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute) |
| <a id="MouseUpEvent"></a>**mouseup** Occurs when the pointing device button is released over an element. | (same) | MouseEvent | [onmouseup](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute) |
| <a id="MouseOverEvent"></a>**mouseover** Occurs when the pointing device is moved onto an element. | (same) | MouseEvent | [onmouseover](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute) |
| <a id="MouseMoveEvent"></a>**mousemove** Occurs when the pointing device is moved while it is over an element. | (same) | MouseEvent | [onmousemove](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute) |
| <a id="MouseOutEvent"></a>**mouseout** Occurs when the pointing device is moved away from an element. | (same) | MouseEvent | [onmouseout](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute) |
| <a id="DOMSubtreeModifiedEvent"></a>**DOMSubtreeModified** This is a general event for notification of all changes to the document. It can be used instead of the more specific events listed below. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="DOMNodeInsertedEvent"></a>**DOMNodeInserted** Fired when a node has been added as a child of another node. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="DOMNodeRemovedEvent"></a>**DOMNodeRemoved** Fired when a node is being removed from another node. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="DOMNodeRemovedFromDocumentEvent"></a>**DOMNodeRemovedFromDocument** Fired when a node is being removed from a document, either through direct removal of the Node or removal of a subtree in which it is contained. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="DOMNodeInsertedIntoDocumentEvent"></a>**DOMNodeInsertedIntoDocument** Fired when a node is being inserted into a document, either through direct insertion of the Node or insertion of a subtree in which it is contained. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="DOMAttrModifiedEvent"></a>**DOMAttrModified** Fired after an attribute has been modified on a node. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="DOMCharacterDataModifiedEvent"></a>**DOMCharacterDataModified** Fired after CharacterData within a node has been modified but the node itself has not been inserted or deleted. (The normative definition of this event is the description in the DOM2 specification.) | (same) | MutationEvent | none |
| <a id="LoadEvent"></a>**SVGLoad** The event is triggered at the point at which the user agent has fully parsed the element and its descendants and is ready to act appropriately upon that element, such as being ready to render the element to the target device. [Referenced external resources that are required](struct.md#ExternalResourcesRequired) must be loaded, parsed and ready to render before the event is triggered. Optional external resources are not required to be ready for the event to be triggered. SVGLoad events do not bubble and are not cancelable. | (same) | none | [onload](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute) |
| <a id="UnloadEvent"></a>**SVGUnload** Only applicable to [outermost svg elements](intro.md#TermOutermostSVGElement). The unload event occurs when the DOM implementation removes a document from a window or frame. SVGUnload events do not bubble and are not cancelable. | (same) | none | [onunload](https://www.w3.org/TR/SVG11/script.html#OnUnloadEventAttribute) |
| <a id="AbortEvent"></a>**SVGAbort** The abort event occurs when page loading is stopped before an element has been allowed to load completely. SVGAbort events bubble but are not cancelable. | (same) | none | [onabort](https://www.w3.org/TR/SVG11/script.html#OnAbortEventAttribute) |
| <a id="ErrorEvent"></a>**SVGError** The error event occurs when an element does not load properly or when an error occurs during script execution. SVGError events bubble but are not cancelable. | (same) | none | [onerror](https://www.w3.org/TR/SVG11/script.html#OnErrorEventAttribute) |
| <a id="ResizeEvent"></a>**SVGResize** Occurs when a document view is being resized. This event is only applicable to [outermost svg elements](intro.md#TermOutermostSVGElement) and is dispatched after the resize operation has taken place. The target of the event is the [‘svg’](struct.md#SVGElement) element. SVGResize events bubble but are not cancelable. | (same) | none | [onresize](https://www.w3.org/TR/SVG11/script.html#OnResizeEventAttribute) |
| <a id="ScrollEvent"></a>**SVGScroll** Occurs when a document view is being shifted along the X or Y or both axis, either through a direct user interaction or any change on the [currentTranslate](struct.md#__svg__SVGSVGElement__currentTranslate) property available on [SVGSVGElement](struct.md#InterfaceSVGSVGElement) interface. This event is only applicable to [outermost svg elements](intro.md#TermOutermostSVGElement) and is dispatched after the shift modification has taken place. The target of the event is the [‘svg’](struct.md#SVGElement) element. SVGScroll events bubble but are not cancelable. | (same) | none | [onscroll](https://www.w3.org/TR/SVG11/script.html#OnScrollEventAttribute) |
| <a id="ZoomEvent"></a>**SVGZoom** Occurs when the zoom level of a document view is being changed, either through a direct user interaction or any change to the [currentScale](struct.md#__svg__SVGSVGElement__currentScale) property available on [SVGSVGElement](struct.md#InterfaceSVGSVGElement) interface. This event is only applicable to [outermost svg elements](intro.md#TermOutermostSVGElement) and is dispatched after the zoom level modification has taken place. The target of the event is the [‘svg’](struct.md#SVGElement) element. SVGZoom events bubble but are not cancelable. | none | none | [onzoom](https://www.w3.org/TR/SVG11/script.html#OnZoomEventAttribute) |
| <a id="BeginEvent"></a>**beginEvent** Occurs when an animation element begins. For details, see the description of Interface TimeEvent in the SMIL Animation specification. | none | none | [onbegin](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute) |
| <a id="EndEvent"></a>**endEvent** Occurs when an animation element ends. For details, see the description of Interface TimeEvent in the SMIL Animation specification. | none | none | [onend](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute) |
| <a id="RepeatEvent"></a>**repeatEvent** Occurs when an animation element repeats. It is raised each time the element repeats, after the first iteration. For details, see the description of Interface TimeEvent in the SMIL Animation specification. | none | none | [onrepeat](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute) |

As in DOM 2 Key events ([[DOM2EVENTS](refs.md#ref-DOM2EVENTS)], section 1.6.3), the SVG specification does not provide a key event set. An event set designed for use with keyboard input devices will be included in a later version of the DOM and SVG specifications.

Details on the parameters passed to event listeners for the event types from DOM2 can be found in the DOM2 specification. For other event types, the parameters passed to event listeners are described elsewhere in this specification.

[Event listener attributes](https://www.w3.org/TR/SVG11/script.html#EventAttributes) can be specified on some elements to listen to a given event. The script in such attributes is run only in response to "bubbling" and "at target" phase events dispatched to the element.

Likewise, [event-value timing specifiers](animate.md#EventValueSyntax) used in [animation element](intro.md#TermAnimationElement) [‘begin’](animate.md#BeginAttribute) and [‘end’](animate.md#EndAttribute) attributes are resolved to concrete times only in response to "bubbling" and "at target" phase events dispatched to the relevant element.

## <a id="UIEvents"></a>16.3 User interface events

On user agents which support interactivity, it is common for authors to define SVG documents such that they are responsive to user interface events. Among the set of possible user events are [pointer events](#PointerEvents), keyboard events, and document events.

In response to user interface (UI) events, the author might start an animation, perform a hyperlink to another Web page, highlight part of the document (e.g., change the color of the graphics elements which are under the pointer), initiate a "roll-over" (e.g., cause some previously hidden graphics elements to appear near the pointer) or launch a script which communicates with a remote database.

## <a id="PointerEvents"></a>16.4 Pointer events

User interface events that occur because of user actions performed on a pointer device are called pointer events.

Many systems support pointer devices such as a mouse or trackball. On systems which use a mouse, pointer events consist of actions such as mouse movements and mouse clicks. On systems with a different pointer device, the pointing device often emulates the behavior of the mouse by providing a mechanism for equivalent user actions, such as a button to press which is equivalent to a mouse click.

For each pointer event, the SVG user agent determines the *target element* of a given pointer event. The target element is the topmost graphics element whose relevant graphical content is under the pointer at the time of the event. (See property [‘pointer-events’](#PointerEventsProperty) for a description of how to determine whether an element's relevant graphical content is under the pointer, and thus in which circumstances that graphic element can be the target element for a pointer event.) When an element is not displayed (i.e., when the [‘display’](painting.md#DisplayProperty) property on that element or one of its ancestors has a value of none), that element cannot be the target of pointer events.

If a target element for the pointer event exists, then the event is dispatched to that element according to the normal event flow ([[DOM2EVENTS](refs.md#ref-DOM2EVENTS)], section 1.2). Note, however, that if the target element is in a [‘use’](struct.md#UseElement) element shadow tree, that the event flow will include [SVGElementInstance](struct.md#InterfaceSVGElementInstance) objects. See [The ‘use’ element](struct.md#UseElement) for details.

If a target element for the pointer event does not exist, then the event is ignored.

## <a id="pointer-processing"></a>16.5 Hit-testing and processing order for user interface events

There are two distinct aspects of pointer-device interaction with an element or area:

1. hit-testing, to determine if a pointer event (such as a mouse movement or mouse click) occurred within the interaction area of an element, and the subsequent DOM event flow;
2. functional processing of actions associated with any relevant element.

### <a id="hit-testing"></a>16.5.1 Hit-testing

Determining whether a pointer event results in a positive [hit-test](intro.md#TermHitTesting) depends upon the position of the pointer, the size and shape of the [graphics element](intro.md#TermGraphicsElement), and the computed value of the [‘pointer-events’](#PointerEventsProperty) property on the element. The definition of the [‘pointer-events’](#PointerEventsProperty) property below describes the exact region that is sensitive to pointer events for a given type of graphics element.

Note that the [‘svg’](struct.md#SVGElement) element is not a [graphics element](intro.md#TermGraphicsElement), and in a [Conforming SVG Stand-Alone File](conform.md#ConformingSVGStandAloneFiles) a [rootmost ‘svg’ element](intro.md#TermRootmostSVGElement) will never be the target of pointer events, though events can bubble to this element. If a pointer event does not result in a positive [hit-test](intro.md#TermHitTesting) on a [graphics element](intro.md#TermGraphicsElement), then it should evoke any user-agent-specific window behavior, such as a presenting a context menu or controls to allow zooming and panning of an SVG document fragment.

This specification does not define the behavior of pointer events on the [rootmost ‘svg’ element](intro.md#TermRootmostSVGElement) for SVG images which are embedded by reference or inclusion within another document, e.g., whether the [rootmost ‘svg’ element](intro.md#TermRootmostSVGElement) embedded in an HTML document intercepts mouse click events; future specifications may define this behavior, but for the purpose of this specification, the behavior is implementation-specific.

### <a id="event-processing"></a>16.5.2 Event processing

An element which is the target of a user interface event may have particular interaction behaviors, depending upon the type of element and whether it has explicit associated interactions, such as scripted event listeners, CSS pseudo-classes matches, or declarative animations with event-based timing. The algorithm and order for processing user interface events for a given target element, after dispatching the DOM event, is as follows:

1. If an event handler registered on this element invokes the `preventDefault()` DOM method, then no further processing for this element is performed, and the event follows the event flow processing as described in DOM Level 2 Events [[DOM2EVENTS](refs.md#ref-DOM2EVENTS)] (or its successor);
2. If the element has an associated title or description, such as a [‘title’](struct.md#TitleElement) element or an [‘xlink:title’](linking.md#XLinkTitleAttribute) attribute, and the user agent supports the display of such information (e.g. via a tooltip or status-bar message), that information should be displayed, as appropriate to the type of pointer event;
3. If the element matches any relevant dynamic pseudo-class selectors appropriate to the type of pointer event, such as `:hover`, `:active`, or `:focus` as described in [[CSS2](refs.md#ref-CSS2)], section 5.11, then the relevant class properties are applied;
4. If the element and the event type are associated with the activation or cancelation of declarative animation though the use of [event-value](animate.md#EventValueSyntax) timing specifiers, any corresponding instance times must be resolved, and any conseqential actions of this instance time resolution (such as immediately starting or stopping the animation) must be performed;
5. If the element is a hyperlink (e.g., it is a descendant element of an [‘a’](linking.md#AElement) element), and the pointer event is of a type that activates that hyperlink (e.g. via a mouse click), and if the hyperlink traversal changes the context of the content (e.g. opens a different document, or moves the pointer away from this element by moving to another part of the same document), then no further processing for this element is performed;
6. If the element is a [text content element](intro.md#TermTextContentElement), and the event type is one which the user agent recognizes as part of a text-selection operation (e.g., a mouse click and drag, or a double-click), then the [text selection](text.md#TextSelection) algorithm is performed;
7. If the event type is one which the user agent associates with the evocation of special user-interface controls (e.g., a right-click or command-click evoking a context menu), the user agent should evoke such user-agent-specific behavior, such as presenting a context menu or controls to allow zooming and panning of an SVG document fragment.

## <a id="PointerEventsProperty"></a>16.6 The ‘pointer-events’ property

In different circumstances, authors may want to control under what conditions particular graphic elements can become the target of pointer events. For example, the author might want a given element to receive pointer events only when the pointer is over the stroked perimeter of a given shape. In other cases, the author might want a given element to ignore pointer events under all circumstances so that graphical elements underneath the given element will become the target of pointer events.

The effects of masking and clipping differ with respect to [pointer-events](#PointerEventsProperty). A clip path is a geometric boundary, and a given point is clearly either inside or outside that boundary; thus, pointer events must be captured normally over the rendered areas of a clipped element, but must not be captured over the clipped areas, as described in the definition of [clipping paths](masking.md#clipPath-geometry). By contrast, a mask is not a binary transition, but a pixel operation, and different behavior for fully transparent and almost-but-not-fully-transparent may be confusingly arbitrary; as a consequence, for elements with a mask applied, pointer-events must still be captured even in areas where the mask goes to zero opacity. If an author wishes to achieve an effect where the transparent parts of a mask allow pointer-events to pass to an element below, a combination of masking and clipping may be used.

The [‘filter’](filters.md#FilterProperty) property has no effect on pointer-events processing, and must in this context be treated as if the [‘filter’](filters.md#FilterProperty) wasn't specified.

For example, suppose a circle with a [‘stroke’](painting.md#StrokeProperty) of red (i.e., the outline is solid red) and a [‘fill’](painting.md#FillProperty) of none (i.e., the interior is not painted) is rendered directly on top of a rectangle with a [‘fill’](painting.md#FillProperty) of blue. The author might want the circle to be the target of pointer events only when the pointer is over the perimeter of the circle. When the pointer is over the interior of the circle, the author might want the underlying rectangle to be the target element of pointer events.

The [‘pointer-events’](#PointerEventsProperty) property specifies under what circumstances a given graphics element can be the target element for a pointer event. It affects the circumstances under which the following are processed:

- user interface events such as mouse clicks
- dynamic pseudo-classes (i.e., :hover, :active and :focus; [[CSS2](refs.md#ref-CSS2)], section 5.11)
- hyperlinks (see [Links out of SVG content: the ‘a’ element](linking.md#Links))

**‘pointer-events’**
:

| *Value:* | visiblePainted \| visibleFill \| visibleStroke \| visible \| painted \| fill \| stroke \| all \| none \| inherit |
| --- | --- |
| *Initial:* | visiblePainted |
| *Applies to:* | [graphics elements](intro.md#TermGraphicsElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**visiblePainted**
: The given element can be the target element for pointer events when the [‘visibility’](painting.md#VisibilityProperty) property is set to visible and when the pointer is over a "painted" area. The pointer is over a painted area if it is over the interior (i.e., fill) of the element and the [‘fill’](painting.md#FillProperty) property has an actual value other than none or it is over the perimeter (i.e., stroke) of the element and the [‘stroke’](painting.md#StrokeProperty) property is set to a value other than none.
**visibleFill**
: The given element can be the target element for pointer events when the [‘visibility’](painting.md#VisibilityProperty) property is set to visible and when the pointer is over the interior (i.e., fill) of the element. The value of the [‘fill’](painting.md#FillProperty) property does not affect event processing.
**visibleStroke**
: The given element can be the target element for pointer events when the [‘visibility’](painting.md#VisibilityProperty) property is set to visible and when the pointer is over the perimeter (i.e., stroke) of the element. The value of the [‘stroke’](painting.md#StrokeProperty) property does not affect event processing.
**visible**
: The given element can be the target element for pointer events when the [‘visibility’](painting.md#VisibilityProperty) property is set to visible and the pointer is over either the interior (i.e., fill) or the perimeter (i.e., stroke) of the element. The values of the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) do not affect event processing.
**painted**
: The given element can be the target element for pointer events when the pointer is over a "painted" area. The pointer is over a painted area if it is over the interior (i.e., fill) of the element and the [‘fill’](painting.md#FillProperty) property has an actual value other than none or it is over the perimeter (i.e., stroke) of the element and the [‘stroke’](painting.md#StrokeProperty) property has an actual value other than none. The value of the [‘visibility’](painting.md#VisibilityProperty) property does not effect event processing.
**fill**
: The given element can be the target element for pointer events when the pointer is over the interior (i.e., fill) of the element. The values of the [‘fill’](painting.md#FillProperty) and [‘visibility’](painting.md#VisibilityProperty) properties do not affect event processing.
**stroke**
: The given element can be the target element for pointer events when the pointer is over the perimeter (i.e., stroke) of the element. The values of the [‘stroke’](painting.md#StrokeProperty) and [‘visibility’](painting.md#VisibilityProperty) properties do not affect event processing.
**all**
: The given element can be the target element for pointer events whenever the pointer is over either the interior (i.e., fill) or the perimeter (i.e., stroke) of the element. The values of the [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty) and [‘visibility’](painting.md#VisibilityProperty) properties do not affect event processing.
**none**
: The given element does not receive pointer events.
For text elements, hit-testing is performed on a character cell basis:

- The value visiblePainted means that the text string can receive events anywhere within the character cell if either the [‘fill’](painting.md#FillProperty) property is set to a value other than none or the [‘stroke’](painting.md#StrokeProperty) property is set to a value other than none, with the additional requirement that the [‘visibility’](painting.md#VisibilityProperty) property is set to visible.
- The values visibleFill, visibleStroke and visible are equivalent and indicate that the text string can receive events anywhere within the character cell if the [‘visibility’](painting.md#VisibilityProperty) property is set to visible. The values of the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties do not affect event processing.
- The value painted means that the text string can receive events anywhere within the character cell if either the [‘fill’](painting.md#FillProperty) property is set to a value other than none or the [‘stroke’](painting.md#StrokeProperty) property is set to a value other than none. The value of the [‘visibility’](painting.md#VisibilityProperty) property does not affect event processing.
- The values fill, stroke and all are equivalent and indicate that the text string can receive events anywhere within the character cell. The values of the [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty) and [‘visibility’](painting.md#VisibilityProperty) properties do not affect event processing.
- The value none indicates that the given text does not receive pointer events.

For raster images, hit-testing is either performed on a whole-image basis (i.e., the rectangular area for the image is one of the determinants for whether the image receives the event) or on a per-pixel basis (i.e., the alpha values for pixels under the pointer help determine whether the image receives the event):

- The value visiblePainted means that the raster image can receive events anywhere within the bounds of the image if any pixel from the raster image which is under the pointer is not fully transparent, with the additional requirement that the [‘visibility’](painting.md#VisibilityProperty) property is set to visible.
- The values visibleFill, visibleStroke and visible are equivalent and indicate that the image can receive events anywhere within the rectangular area for the image if the [‘visibility’](painting.md#VisibilityProperty) property is set to visible.
- The value painted means that the raster image can receive events anywhere within the bounds of the image if any pixel from the raster image which is under the pointer is not fully transparent. The value of the [‘visibility’](painting.md#VisibilityProperty) property does not affect event processing.
- The values fill, stroke and all are equivalent and indicate that the image can receive events anywhere within the rectangular area for the image. The value of the [‘visibility’](painting.md#VisibilityProperty) property does not affect event processing.
- The value none indicates that the image does not receive pointer events.

Note that for raster images, the values of properties [‘opacity’](masking.md#OpacityProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) do not affect event processing.

## <a id="EnableZoomAndPanControls"></a>16.7 Magnification and panning

<a id="ZoomAndPanAttribute"></a>Magnification represents a complete, uniform transformation on an SVG document fragment, where the magnify operation scales all graphical elements by the same amount. A magnify operation has the effect of a supplemental scale and translate transformation placed at the outermost level on the SVG document fragment (i.e., outside the [outermost svg element](intro.md#TermOutermostSVGElement)).

Panning represents a translation (i.e., a shift) transformation on an SVG document fragment in response to a user interface action.

SVG user agents that operate in interaction-capable user environments are required to support the ability to magnify and pan.

The [outermost svg element](intro.md#TermOutermostSVGElement) in an SVG document fragment has attribute [‘zoomAndPan’](#ZoomAndPanAttribute), which takes the possible values of *disable* and *magnify*, with the default being *magnify*.

If *disable*, the user agent shall disable any magnification and panning controls and not allow the user to magnify or pan on the given document fragment.

If *magnify*, in environments that support user interactivity, the user agent shall provide controls to allow the user to perform a "magnify" operation on the document fragment.

If a [‘zoomAndPan’](#ZoomAndPanAttribute) attribute is assigned to an inner [‘svg’](struct.md#SVGElement) element, the [‘zoomAndPan’](#ZoomAndPanAttribute) setting on the inner [‘svg’](struct.md#SVGElement) element will have no effect on the SVG user agent.

[Animatable](animate.md#Animatable): no.

## <a id="Cursors"></a>16.8 Cursors

### <a id="CursorIntro"></a>16.8.1 Introduction to cursors

Some interactive display environments provide the ability to modify the appearance of the pointer, which is also known as the *cursor*. Three types of cursors are available:

- Standard built-in cursors
- Platform-specific custom cursors
- Platform-independent custom cursors

The [‘cursor’](#CursorProperty) property is used to specify which cursor to use. The [‘cursor’](#CursorProperty) property can be used to reference standard built-in cursors by specifying a keyword such as *crosshair* or a custom cursor. Custom cursors are referenced via a <funciri> and can point to either an external resource such as a platform-specific cursor file or to a [‘cursor’](#CursorElement) element, which can be used to define a platform-independent cursor.

### <a id="CursorProperty"></a>16.8.2 The ‘cursor’ property

**‘cursor’**
:

| *Value:* | [ [[<funciri>](types.md#DataTypeFuncIRI) ,]* [ auto \| crosshair \| default \| pointer \| move \| e-resize \| ne-resize \| nw-resize \| n-resize \| se-resize \| sw-resize \| s-resize \| w-resize\| text \| wait \| help ] ] \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [container elements](intro.md#TermContainerElement) and [graphics elements](intro.md#TermGraphicsElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual, interactive |
| *[Animatable](animate.md#Animatable):* | yes |

This property specifies the type of cursor to be displayed for the pointing device. Values have the following meanings:

**auto**
: The UA determines the cursor to display based on the current context.
**crosshair**
: A simple crosshair (e.g., short line segments resembling a "+" sign).
**default**
: The platform-dependent default cursor. Often rendered as an arrow.
**pointer**
: The cursor is a pointer that indicates a link.
**move**
: Indicates something is to be moved.
**e-resize, ne-resize, nw-resize, n-resize, se-resize, sw-resize, s-resize, w-resize**
: Indicate that some edge is to be moved. For example, the 'se-resize' cursor is used when the movement starts from the south-east corner of the box.
**text**
: Indicates text that can be selected. Often rendered as an I-bar.
**wait**
: Indicates that the program is busy. Often rendered as a watch or hourglass.
**help**
: Help is available for the object under the cursor. Often rendered as a question mark or a balloon.
**<funciri>**
: The user agent retrieves the cursor from the resource designated by the URI. If the user agent cannot handle the first cursor of a list of cursors, it shall attempt to handle the second, etc. If the user agent cannot handle any user-defined cursor, it must use the generic cursor at the end of the list.
```
P { cursor : url("mything.cur"), url("second.svg#curs"), text; }
```

The [‘cursor’](#CursorProperty) property for SVG is identical to the ‘cursor’ property defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 18.1), with the additional requirement that SVG user agents must support cursors defined by the SVG [‘cursor’](#CursorElement) element. This gives a single, cross-platform, interoperable cursor format, with PNG as the raster component.

### <a id="CursorElement"></a>16.8.3 The ‘cursor’ element

The [‘cursor’](#CursorElement) element can be used to define a platform-independent custom cursor. A recommended approach for defining a platform-independent custom cursor is to create a PNG image [[PNG](refs.md#ref-PNG)] and define a [‘cursor’](#CursorElement) element that references the PNG image and identifies the exact position within the image which is the pointer position (i.e., the hot spot).

The PNG format is recommended because it supports the ability to define a transparency mask via an alpha channel. If a different image format is used, this format should support the definition of a transparency mask (two options: provide an explicit alpha channel or use a particular pixel color to indicate transparency). If the transparency mask can be determined, the mask defines the shape of the cursor; otherwise, the cursor is an opaque rectangle. Typically, the other pixel information (e.g., the R, G and B channels) defines the colors for those parts of the cursor which are not masked out. Note that cursors usually contain at least two colors so that the cursor can be visible over most backgrounds.

‘cursor’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#CursorElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘x’](#CursorElementXAttribute)
- [‘y’](#CursorElementYAttribute)
- [‘xlink:href’](#CursorElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGCursorElement](#InterfaceSVGCursorElement)

*Attribute definitions:*

**<a id="CursorElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The *x-coordinate* of the position in the cursor's coordinate system which represents the precise position that is being pointed to. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="CursorElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The *y-coordinate* of the position in the cursor's coordinate system which represents the precise position that is being pointed to. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="CursorElementHrefAttribute"></a>xlink:href = "[<funciri>](types.md#DataTypeFuncIRI)"**
: A [Functional IRI reference](types.md#DataTypeFuncIRI) to the file or element which provides the image of the cursor. [Animatable](animate.md#Animatable): yes.
SVG user agents are required to support PNG format images as targets of the [‘xlink:href’](#CursorElementHrefAttribute) attribute.

## <a id="DOMInterfaces"></a>16.9 DOM interfaces

### <a id="InterfaceSVGCursorElement"></a>16.9.1 Interface SVGCursorElement

The [SVGCursorElement](#InterfaceSVGCursorElement) interface corresponds to the [‘cursor’](#CursorElement) element.

```
interface SVGCursorElement : SVGElement,
                             SVGURIReference,
                             SVGTests,
                             SVGExternalResourcesRequired {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
};
```

**Attributes:**
:

**<a id="__svg__SVGCursorElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#CursorElementXAttribute) on the given [‘cursor’](#CursorElement) element.

**<a id="__svg__SVGCursorElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#CursorElementYAttribute) on the given [‘cursor’](#CursorElement) element.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](filters.md) ⋅ [Next](linking.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
