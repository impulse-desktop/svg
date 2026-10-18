<!-- https://www.w3.org/TR/SVG11/animate.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](https://www.w3.org/TR/SVG11/script.html) ⋅ [Next](fonts.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 19 Animation

## <a id="Introduction"></a>19.1 Introduction

Because the Web is a dynamic medium, SVG supports the ability to change vector graphics over time. SVG content can be animated in the following ways:

- Using SVG's [animation elements](#AnimationElements). SVG document fragments can describe time-based modifications to the document's elements. Using the various animation elements, you can define motion paths, fade-in or fade-out effects, and objects that grow, shrink, spin or change color.
- Using the [SVG DOM](https://www.w3.org/TR/SVG11/svgdom.html). The SVG DOM conforms to key aspects of the Document Object Model (DOM) Level 1 [[DOM1](refs.md#ref-DOM1)] and Document Object Model (DOM) Level 2 [[DOM2](refs.md#ref-DOM2)] specifications. Every attribute and style sheet setting is accessible to scripting, and SVG offers a set of additional DOM interfaces to support efficient animation via scripting. As a result, virtually any kind of animation can be achieved. The timer facilities in scripting languages such as ECMAScript can be used to start up and control the animations [[ECMA-262](refs.md#ref-ECMA-262)]. (See [example](#DOMAnimationExample) below.)
- SVG has been designed to allow SMIL [[SMIL](refs.md#ref-SMIL)] to use animated or static SVG content as media components.

## <a id="AnimationElements"></a>19.2 Animation elements

### <a id="AnimationElementsIntro"></a>19.2.1 Overview

SVG's animation elements were developed in collaboration with the W3C Synchronized Multimedia (SYMM) Working Group, developers of the Synchronized Multimedia Integration Language (SMIL) 3.0 Specification [[SMIL](refs.md#ref-SMIL)].

The SYMM Working Group, in collaboration with the SVG Working Group, has authored the SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)], which represents a general-purpose XML animation feature set. SVG incorporates the animation features defined in the SMIL Animation specification and provides some SVG-specific extensions.

For an introduction to the approach and features available in any language that supports SMIL Animation, see SMIL Animation overview and SMIL Animation animation model ([[SMILANIM](refs.md#ref-SMILANIM)], sections 2 and 3). For the list of animation features which go beyond SMIL Animation, see [SVG extensions to SMIL Animation](#SVGExtensionsToSMILAnimation).

### <a id="RelationshipToSMILAnimation"></a>19.2.2 Relationship to SMIL Animation

SVG is a host language in terms of SMIL Animation and therefore introduces additional constraints and features as permitted by that specification. Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for SVG's animation elements and attributes is the SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)].

SVG supports the following four animation elements which are defined in the SMIL Animation specification:

| [‘animate’](#AnimateElement) | allows scalar attributes and properties to be assigned different values over time |
| --- | --- |
| [‘set’](#SetElement) | a convenient shorthand for [‘animate’](#AnimateElement), which is useful for assigning animation values to non-numeric attributes and properties, such as the [‘visibility’](painting.md#VisibilityProperty) property |
| [‘animateMotion’](#AnimateMotionElement) | moves an element along a motion path |
| [‘animateColor’](#AnimateColorElement) | modifies the color value of particular attributes or properties over time |

Although SVG defines [‘animateColor’](#AnimateColorElement), its use is deprecated in favor of simply using the [‘animate’](#AnimateElement) element to target properties that can take color values.

<a id="SVGExtensionsToSMILAnimation"></a>Additionally, SVG includes the following compatible extensions to SMIL Animation:

| [‘animateTransform’](#AnimateTransformElement) | modifies one of SVG's transformation attributes over time, such as the [‘transform’](coords.md#TransformAttribute) attribute |
| --- | --- |
| [‘path’](#PathAttribute) attribute | SVG allows any feature from SVG's [path data](paths.md#PathData) syntax to be specified in a [‘path’](#PathAttribute) attribute to the [‘animateMotion’](#AnimateMotionElement) element (SMIL Animation only allows a subset of SVG's path data syntax within a [‘path’](#PathAttribute) attribute) |
| [‘mpath’](#MPathElement) element | SVG allows an [‘animateMotion’](#AnimateMotionElement) element to contain a child [‘mpath’](#MPathElement) element which references an SVG [‘path’](paths.md#PathElement) element as the definition of the motion path |
| [‘keyPoints’](#KeyPointsAttribute) attribute | SVG adds a [‘keyPoints’](#KeyPointsAttribute) attribute to the [‘animateMotion’](#AnimateMotionElement) to provide precise control of the velocity of motion path animations |
| [‘rotate’](#RotateAttribute) attribute | SVG adds a [‘rotate’](#RotateAttribute) attribute to the [‘animateMotion’](#AnimateMotionElement) to control whether an object is automatically rotated so that its x-axis points in the same direction (or opposite direction) as the directional tangent vector of the motion path |

For compatibility with other aspects of the language, SVG uses [IRI references](linking.md#IRIReference) via an [‘xlink:href’](#HrefAttribute) attribute to identify the elements which are to be targets of the animations, as allowed in SMIL 3.0.

SMIL Animation requires that the host language define the meaning for <a id="DocumentBegin"></a>document begin and the <a id="DocumentEnd"></a>document end. Since an [‘svg’](struct.md#SVGElement) is sometimes the root of the XML document tree and other times can be a component of a parent XML grammar, the *document begin* for a given SVG document fragment is defined to be the exact time at which the [‘svg’](struct.md#SVGElement) element's [SVGLoad event](interact.md#LoadEvent) is triggered. The *document end* of an SVG document fragment is the point at which the document fragment has been released and is no longer being processed by the user agent. However, nested [‘svg’](struct.md#SVGElement) elements within an SVG document do not constitute document fragments in this sense, and do not define a separate document begin; all times within the nested SVG fragment are relative to the document time defined for the root [‘svg’](struct.md#SVGElement) element.

For SVG, the term <a id="PresentationTime"></a>presentation time indicates the position in the timeline relative to the *document begin* of a given document fragment.

SVG defines more constrained error processing than is defined in the SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)]. SMIL Animation defines error processing behavior where the document continues to run in certain error situations, whereas all animations within an SVG document fragment will stop in the event of any error within the document (see [Error processing](implnote.md#ErrorProcessing)).

### <a id="AnimationElementsExample"></a>19.2.3 Animation elements example

<a id="ExampleAnim01"></a>Example anim01 below demonstrates each of SVG's five animation elements.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="3cm"  viewBox="0 0 800 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example anim01 - demonstrate animation elements</desc>
  <rect x="1" y="1" width="798" height="298" 
        fill="none" stroke="blue" stroke-width="2" />
  <!-- The following illustrates the use of the 'animate' element
        to animate a rectangles x, y, and width attributes so that
        the rectangle grows to ultimately fill the viewport. -->
  <rect id="RectElement" x="300" y="100" width="300" height="100"
        fill="rgb(255,255,0)"  >
    <animate attributeName="x" attributeType="XML"
             begin="0s" dur="9s" fill="freeze" from="300" to="0" />
    <animate attributeName="y" attributeType="XML"
             begin="0s" dur="9s" fill="freeze" from="100" to="0" />
    <animate attributeName="width" attributeType="XML"
             begin="0s" dur="9s" fill="freeze" from="300" to="800" />
    <animate attributeName="height" attributeType="XML"
             begin="0s" dur="9s" fill="freeze" from="100" to="300" />
  </rect>
  <!-- Set up a new user coordinate system so that
        the text string's origin is at (0,0), allowing
        rotation and scale relative to the new origin -->
  <g transform="translate(100,100)" >
    <!-- The following illustrates the use of the 'set', 'animateMotion',
         'animate' and 'animateTransform' elements. The 'text' element 
         below starts off hidden (i.e., invisible). At 3 seconds, it:
           * becomes visible
           * continuously moves diagonally across the viewport
           * changes color from blue to dark red
           * rotates from -30 to zero degrees
           * scales by a factor of three. -->
    <text id="TextElement" x="0" y="0"
          font-family="Verdana" font-size="35.27" visibility="hidden"  > 
      It's alive!
      <set attributeName="visibility" attributeType="CSS" to="visible"
           begin="3s" dur="6s" fill="freeze" />
      <animateMotion path="M 0 0 L 100 100" 
           begin="3s" dur="6s" fill="freeze" />
      <animate attributeName="fill" attributeType="CSS"
           from="rgb(0,0,255)" to="rgb(128,0,0)"
           begin="3s" dur="6s" fill="freeze" />
      <animateTransform attributeName="transform" attributeType="XML"
           type="rotate" from="-30" to="0"
           begin="3s" dur="6s" fill="freeze" />
      <animateTransform attributeName="transform" attributeType="XML"
           type="scale" from="1" to="3" additive="sum"
           begin="3s" dur="6s" fill="freeze" />
    </text>
  </g>
</svg>
```

Example anim01

| [image: Example anim01 - at zero seconds] At zero seconds |  | [image: Example anim01 - at three seconds] At three seconds |
| --- | --- | --- |
| [image: Example anim01 - at six seconds] At six seconds |  | [image: Example anim01 - at nine seconds] At nine seconds |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/animate/anim01.svg)

The sections below describe the various animation attributes and elements.

### <a id="TargetElement"></a>19.2.4 Attributes to identify the target element for an animation

The following attribute is common to all animation elements and identifies the target element for the animation.

*Attribute definitions:*

**<a id="HrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
:

An [IRI reference](intro.md#TermIRIReference) to the element which is the target of this animation and which therefore will be modified over time.

The target element must be part of the [current SVG document fragment](intro.md#TermCurrentSVGDocumentFragment).

<iri> must point to exactly one target element which is capable of being the target of the given animation. If <iri> points to multiple target elements, if the given target element is not capable of being a target of the given animation, or if the given target element is not part of the current SVG document fragment, then the document is in error (see [Error processing](implnote.md#ErrorProcessing)).

If the [‘xlink:href’](#HrefAttribute) attribute is not provided, then the target element will be the immediate parent element of the current animation element.

Refer to the descriptions of the individual animation elements for any restrictions on what types of elements can be targets of particular types of animations.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: Specifying the animation target ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.1).

### <a id="TargetAttributes"></a>19.2.5 Attributes to identify the target attribute or property for an animation

The following attributes are the animation attribute target attributes, which identify the target attribute or property for the given [target element](#TargetElement) whose value changes over time.

*Attribute definitions:*

**<a id="AttributeNameAttribute"></a>attributeName = "<attributeName>"**
:

Specifies the name of the target attribute. An XMLNS prefix may be used to indicate the XML namespace for the attribute. The prefix will be interpreted in the scope of the current (i.e., the referencing) animation element.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: Specifying the animation target ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.1).

**<a id="AttributeTypeAttribute"></a>attributeType = "CSS | XML | auto"**
:

Specifies the namespace in which the target attribute and its associated values are defined. The attribute value is one of the following (values are case-sensitive):

**CSS**
: This specifies that the value of [‘attributeName’](#AttributeNameAttribute) is the name of a CSS property defined as animatable in this specification.
**XML**
: This specifies that the value of [‘attributeName’](#AttributeNameAttribute) is the name of an XML attribute defined in the default XML namespace for the target element. If the value for [‘attributeName’](#AttributeNameAttribute) has an XMLNS prefix, the implementation must use the associated namespace as defined in the scope of the target element. The attribute must be defined as animatable in this specification.
**auto**
: The implementation should match the [‘attributeName’](#AttributeNameAttribute) to an attribute for the target element. The implementation must first search through the list of CSS properties for a matching property name, and if none is found, search the default XML namespace for the element.
The default value is 'auto'.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: Specifying the animation target ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.1).

### <a id="Animation.nsexample"></a>19.2.6 Animation with namespaces

<a id="ExampleNS01"></a>Example animns01 below shows a namespace prefix being resolved to a namespace name in the scope of the referencing element, and that namespace name being used (regardless of the prefix which happens to be used in the target scope) to identify the attribute being animated.

```
<?xml version="1.0" encoding="UTF-8"?>
<svg version="1.1" xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink">
  <title>Demonstration of the resolution of namespaces for animation</title>
  <!-- at the point of definition, the QName a:href resolves to the namespace
       name "http://www.w3.org/1999/xlink" and the local name "href" -->
  <g xmlns:a="http://www.w3.org/1999/xlink">
    <animate attributeName="a:href" xlink:href="#foo" dur="2s" to="two.png" fill="freeze"/>
  </g>
  <!-- at the point of use, the namespace name "http://www.w3.org/1999/xlink"
       happens to be bound to the namespace prefix 'b' while the prefix
       'xlink' is bound to a different namespace name -->
  <g xmlns:b="http://www.w3.org/1999/xlink" xmlns:xlink="http://example.net/bar">
    <image xml:id="foo" b:href="one.png" x="35" y="50" width="410" height="160"/>
  </g>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/animate/animns01.svg)

### <a id="complexDistances"></a>19.2.7 Paced animation and complex types

Paced animations assume a notion of distance between the various animation values defined by the [‘to’](#ToAttribute), [‘from’](#FromAttribute), [‘by’](#ByAttribute) and [‘values’](#ValuesAttribute) attributes. Distance is defined only for scalar types (such as [<length>](types.md#DataTypeLength)), colors and the subset of transformation types that are supported by [‘animateTransform’](#AnimateTransformElement). In the list of distance functions below, V_a and V_b represent the two values the distance between which is being calculated.

Since paced animation is intended to produce an animation with an even pace of change, it does not make sense to define distance functions for all data types. Distance can be usefully defined for types whose values are *n*-dimensional vectors (including scalars, which are 1-dimensional vectors). For example, a [<length>](types.md#DataTypeLength) value is a scalar value, and a [<color>](types.md#DataTypeColor) value is a 3-dimensional vector. Thus attributes of these types can have paced animation applied to them. On the other hand, a [<list-of-length>](types.md#DataTypeLengths) (as used by [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty)) is a list of scalars (1-dimensional vectors), and [<list-of-points>](shapes.md#PointsBNF) (as used by the [‘points’](shapes.md#PolygonElementPointsAttribute) attribute on a [‘polygon’](shapes.md#PolygonElement)) is a list of 2-dimensional vectors. Therefore, these types do not have a distance function defined and cannot have paced animation applied to them.

The distance functions for types that support paced animation are as follows:

** [<coordinate>](types.md#DataTypeCoordinate), [<integer>](types.md#DataTypeInteger), [<length>](types.md#DataTypeLength) and [<number>](types.md#DataTypeNumber) **
:

distance(V_a, V_b) = |V_a − V_b|

Examples: animating the [‘x’](shapes.md#RectElementXAttribute) attribute on a [‘rect’](shapes.md#RectElement), or the [‘stroke-width’](painting.md#StrokeWidthProperty) property on a [‘circle’](shapes.md#CircleElement).

** [<color>](types.md#DataTypeColor) **
:

distance(V_a, V_b) = sqrt((V_a.red − V_b.red)^2 + (V_a.green − V_b.green)^2 + (V_a.blue − V_b.blue)^2), where:

V_*i*.red is the red component of the V_*i* color value,

V_*i*.green is the green component of the V_*i* color value, and

V_*i*.blue is the blue component of the V_*i* color value.

Each of the color component values is usually in the range [0, 1], where 0 represents none of that color component, and 1 represents the maximum amount of that color component, in the sRGB gamut [[SRGB](refs.md#ref-SRGB)]. Since [<color>](types.md#DataTypeColor) values may specify colors outside of the sRGB gamut, these component values may lie outside the range [0, 1].

Example: animating the [‘fill’](painting.md#FillProperty) property on an [‘ellipse’](shapes.md#EllipseElement).

** Transform definitions of type 'translate' **
:

distance(V_a, V_b) = sqrt((V_a.tx − V_b.tx)^2 + (V_a.ty − V_b.ty)^2), where:

V_*i*.tx is the *x* component of the V_*i* translation transform value, and

V_*i*.ty is the *y* component of the V_*i* translation transform value.

Example (for all transform definition types): animating the [‘transform’](coords.md#TransformAttribute) attribute on a [‘g’](struct.md#GElement) using [‘animateTransform’](#AnimateTransformElement).

** Transform definitions of type 'scale' **
:

distance(V_a, V_b) = sqrt((V_a.sx − V_b.sx)^2 + (V_a.sy − V_b.sy)^2), where:

V_*i*.sx is the *x* component of the V_*i* scale transform value, and

V_*i*.sy is the *y* component of the V_*i* scale transform value.

Note that, as when specifying scale transformations in a [<transform-list>](types.md#DataTypeTransformList), if the *y* component of the scale is omitted it is implicitly equal to the *x* component.

** Transform definitions of type 'rotate', 'skewX' and 'skewY' **
:

distance(V_a, V_b) = sqrt((V_a.angle − V_b.angle)^2), where:

V_*i*.angle is the angle component of the V_*i* rotation or skew transform value.

Since the distance function for rotations is not in terms of the rotation center point components, a paced animation that changes the rotation center point may not appear to have a paced movement when the animation is applied.

Distance functions for all other data types are not defined. If calcMode="paced" is used on an animation of an attribute or property whose type is not one of those listed above, the animation effect is undefined. [SVG user agents](intro.md#TermSVGUserAgent) may choose to perform the animation as if calcMode="linear", but this is not required. Authors are recommended not to specify paced animation on types not listed above.

### <a id="TimingAttributes"></a>19.2.8 Attributes to control the timing of the animation

The following attributes are the animation timing attributes. They are common to all animation elements and control the timing of the animation, including what causes the animation to start and end, whether the animation runs repeatedly, and whether to retain the end state the animation once the animation ends.

<a id="optWSP"></a>In the syntax specifications that follow, optional white space is indicated as "S", defined as follows:

```
S ::= (#x20 | #x9 | #xD | #xA)*
```

*Attribute definitions:*

**<a id="BeginAttribute"></a>begin = "[begin-value-list](#BeginValueListSyntax)"**
:

Defines when the element should begin (i.e. become active).

The attribute value is a semicolon separated list of values.

**<a id="BeginValueListSyntax"></a>begin-value-list ::= [begin-value](#BeginValueSyntax) (S? ";" S? begin-value-list )?**
: A semicolon separated list of begin values. The interpretation of a list of begin times is detailed in SMIL Animation's section on "Evaluation of begin and end time lists".
**<a id="BeginValueSyntax"></a>begin-value ::= ( [offset-value](#OffsetValueSyntax) | [syncbase-value](#SyncbaseValueSyntax) | [event-value](#EventValueSyntax) | [repeat-value](#RepeatValueSyntax) | [accessKey-value](#AccessKeyValueSyntax) | [wallclock-sync-value](#WallclockSyncValueSyntax) | ["indefinite"](#IndefiniteValue) )**
: Describes the element begin.
**<a id="OffsetValueSyntax"></a>offset-value ::= ( S? "+" | "-" S? )? ( [Clock-value](#ClockValueSyntax) )**
: For SMIL Animation, this describes the element begin as an offset from an implicit syncbase. For SVG, the implicit syncbase begin is defined to be relative to the document begin. Negative begin times are entirely valid and easy to compute, as long as there is a resolved document begin time.
**<a id="SyncbaseValueSyntax"></a>syncbase-value ::= ( Id-value "." ( "begin" | "end" ) ) ( S? ("+"|"-") S? [Clock-value](#ClockValueSyntax) )?**
: Describes a syncbase and an optional offset from that syncbase. The element begin is defined relative to the begin or active end of another animation. A syncbase consists of an ID reference to another animation element followed by either `begin` or `end` to identify whether to synchronize with the beginning or active end of the referenced animation element.
**<a id="EventValueSyntax"></a>event-value ::= ( Id-value "." )? ( event-ref ) ( S? ("+"|"-") S? [Clock-value](#ClockValueSyntax) )?**
: Describes an event and an optional offset that determine the element begin. The animation begin is defined relative to the time that the event is raised. The list of event-symbols available for a given event-base element is the list of event attributes available for the given element as defined by the [SVG DTD](https://www.w3.org/TR/SVG11/svgdtd.html), with the one difference that the leading 'on' is removed from the event name (i.e., the animation event name is 'click', not 'onclick'). A list of all events supported by SVG can be found in [Complete list of supported events](interact.md#SVGEvents). Details of event-based timing are described in SMIL Animation: Unifying Event-based and Scheduled Timing.
**<a id="RepeatValueSyntax"></a>repeat-value ::= ( Id-value "." )? "repeat(" integer ")" ( S? ("+"|"-") S? [Clock-value](#ClockValueSyntax) )?**
: Describes a qualified repeat event. The element begin is defined relative to the time that the repeat event is raised with the specified iteration value.
**<a id="AccessKeyValueSyntax"></a>accessKey-value ::= "accessKey(" character ")" ( S? ("+"|"-") S? [Clock-value](#ClockValueSyntax) )?**
: Describes an accessKey that determines the element begin. The element begin is defined relative to the time that the accessKey character is input by the user.
**<a id="WallclockSyncValueSyntax"></a>wallclock-sync-value ::= "wallclock(" wallclock-value ")"**
: Describes the element begin as a real-world clock time. The wallclock time syntax is based upon syntax defined in Representation of dates and times [[ISO8601](refs.md#ref-ISO8601)].
**<a id="IndefiniteValue"></a>"indefinite"**
:

The begin of the animation will be determined by a "beginElement()" method call or a hyperlink targeted to the element.

The animation DOM methods are described in [DOM interfaces](#DOMInterfaces).

Hyperlink-based timing is described in SMIL Animation: Hyperlinks and timing.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'begin' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.1).

**<a id="DurAttribute"></a>dur = [Clock-value](#ClockValueSyntax) | "media" | "indefinite"**
:

Specifies the simple duration.

The attribute value can be one of the following:

**[Clock-value](#ClockValueSyntax)**
: Specifies the length of the simple duration in [presentation time](#PresentationTime). Value must be greater than 0.
**"media"**
: Specifies the simple duration as the intrinsic media duration. This is only valid for elements that define media. (For SVG's [animation elements](#AnimationElements), if 'media' is specified, the attribute will be ignored.)
**"indefinite"**
: Specifies the simple duration as indefinite.
If the animation does not have a [‘dur’](#DurAttribute) attribute, the simple duration is indefinite. Note that interpolation will not work if the simple duration is indefinite (although this may still be useful for [‘set’](#SetElement) elements). Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'dur' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.1).

**<a id="EndAttribute"></a>end = "[end-value-list](#EndValueListSyntax)"**
:

Defines an end value for the animation that can constrain the active duration. The attribute value is a semicolon separated list of values.

**<a id="EndValueListSyntax"></a>end-value-list ::= [end-value](#EndValueSyntax) (S? ";" S? end-value-list )?**
: A semicolon separated list of end values. The interpretation of a list of end times is detailed below.
**<a id="EndValueSyntax"></a>end-value ::= ( [offset-value](#OffsetValueSyntax) | [syncbase-value](#SyncbaseValueSyntax) | [event-value](#EventValueSyntax) | [repeat-value](#RepeatValueSyntax) | [accessKey-value](#AccessKeyValueSyntax) | [wallclock-sync-value](#WallclockSyncValueSyntax) | "indefinite" )**
: Describes the active end of the animation.
A value of 'indefinite' specifies that the end of the animation will be determined by an [endElement](#__smil__ElementTimeControl__endElement) method call (the animation DOM methods are described in [DOM interfaces](#DOMInterfaces)).

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'end' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.2).

**<a id="MinAttribute"></a>min = [Clock-value](#ClockValueSyntax) | "media"**
:

Specifies the minimum value of the active duration.

The attribute value can be either of the following:

**[Clock-value](#ClockValueSyntax)**
:

Specifies the length of the minimum value of the active duration, measured in local time.

Value must be greater than 0.

**"media"**
: Specifies the minimum value of the active duration as the intrinsic media duration. This is only valid for elements that define media. (For SVG's [animation elements](#AnimationElements), if 'media' is specified, the attribute will be ignored.)
The default value for [‘min’](#MinAttribute) is '0'. This does not constrain the active duration at all.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'min' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.3).

**<a id="MaxAttribute"></a>max = [Clock-value](#ClockValueSyntax) | "media"**
:

Specifies the maximum value of the active duration.

The attribute value can be either of the following:

**[Clock-value](#ClockValueSyntax)**
:

Specifies the length of the maximum value of the active duration, measured in local time.

Value must be greater than 0.

**"media"**
: Specifies the maximum value of the active duration as the intrinsic media duration. This is only valid for elements that define media. (For SVG's [animation elements](#AnimationElements), if 'media' is specified, the attribute will be ignored.)
There is no default value for [‘max’](#MaxAttribute). This does not constrain the active duration at all.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'max' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.3).

**<a id="RestartAttribute"></a>restart = "always" | "whenNotActive" | "never"**
:

**always**
: The animation can be restarted at any time.  This is the default value.
**whenNotActive**
: The animation can only be restarted when it is not active (i.e. after the active end). Attempts to restart the animation during its active duration are ignored.
**never**
: The element cannot be restarted for the remainder of the current simple duration of the parent time container. (In the case of SVG, since the parent time container is the SVG document fragment, then the animation cannot be restarted for the remainder of the document duration.)
Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'restart' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.7).

**<a id="RepeatCountAttribute"></a>repeatCount = numeric value | "indefinite"**
:

Specifies the number of iterations of the animation function. It can have the following attribute values:

**numeric value**
: This is a (base 10) "floating point" numeric value that specifies the number of iterations. It can include partial iterations expressed as fraction values. A fractional value describes a portion of the simple duration. Values must be greater than 0.
**"indefinite"**
: The animation is defined to repeat indefinitely (i.e. until the document ends).
Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'repeatCount' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.1).

**<a id="RepeatDurAttribute"></a>repeatDur = [Clock-value](#ClockValueSyntax) | "indefinite"**
:

Specifies the total duration for repeat. It can have the following attribute values:

**[Clock-value](#ClockValueSyntax)**
: Specifies the duration in [presentation time](#PresentationTime) to repeat the animation function `**f(t)**`.
**"indefinite"**
: The animation is defined to repeat indefinitely (i.e. until the document ends).
Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'repeatDur' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.1).

**<a id="FillAttribute"></a>fill = "freeze" | "remove"**
:

This attribute can have the following values:

**freeze**
: The animation effect F(t) is defined to freeze the effect value at the last value of the active duration. The animation effect is "frozen" for the remainder of the document duration (or until the animation is restarted - see SMIL Animation: Restarting animation).
**remove**
:

The animation effect is removed (no longer applied) when the active duration of the animation is over. After the active end of the animation, the animation no longer affects the target (unless the animation is restarted - see SMIL Animation: Restarting animation).

This is the default value.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'fill' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.5).

The SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)] defines the detailed processing rules associated with the above attributes. Except for any SVG-specific rules explicitly mentioned in this specification, the SMIL Animation specification is the normative definition of the processing rules for the above attributes.

#### <a id="ClockValueSyntax"></a>19.2.8.1 Clock values

Clock values have the same syntax as in SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)]. The grammar for clock values is repeated here:

```
Clock-val         ::= Full-clock-val | Partial-clock-val 
                      | Timecount-val
Full-clock-val    ::= Hours ":" Minutes ":" Seconds ("." Fraction)?
Partial-clock-val ::= Minutes ":" Seconds ("." Fraction)?
Timecount-val     ::= Timecount ("." Fraction)? (Metric)?
Metric            ::= "h" | "min" | "s" | "ms"
Hours             ::= DIGIT+; any positive number
Minutes           ::= 2DIGIT; range from 00 to 59
Seconds           ::= 2DIGIT; range from 00 to 59
Fraction          ::= DIGIT+
Timecount         ::= DIGIT+
2DIGIT            ::= DIGIT DIGIT
DIGIT             ::= [0-9]
```

For Timecount values, the default metric suffix is "s" (for seconds). No embedded white space is allowed in clock values, although leading and trailing white space characters will be ignored.

Clock values describe [presentation time](#PresentationTime).

The following are examples of legal clock values:

- Full clock values: `   02:30:03   ` = 2 hours, 30 minutes and 3 seconds `  50:00:10.25` = 50 hours, 10 seconds and 250 milliseconds
- Partial clock value: `   02:33  ` = 2 minutes and 33 seconds `  00:10.5` = 10.5 seconds = 10 seconds and 500 milliseconds
- Timecount values: `  3.2h   ` = 3.2 hours = 3 hours and 12 minutes `  45min  ` = 45 minutes `  30s    ` = 30 seconds `  5ms    ` = 5 milliseconds `  12.467 ` = 12 seconds and 467 milliseconds

Fractional values are just (base 10) floating point definitions of seconds. Thus:

`00.5s = 500 milliseconds 00:00.005 = 5 milliseconds`

### <a id="ValueAttributes"></a>19.2.9 Attributes that define animation values over time

The following attributes are the animation value attributes. They are common to elements [‘animate’](#AnimateElement), [‘animateColor’](#AnimateColorElement), [‘animateMotion’](#AnimateMotionElement) and [‘animateTransform’](#AnimateTransformElement). These attributes define the values that are assigned to the target attribute or property over time. The attributes below provide control over the relative timing of keyframes and the interpolation method between discrete values.

*Attribute definitions:*

**<a id="CalcModeAttribute"></a>calcMode = "discrete | linear | paced | spline"**
:

Specifies the interpolation mode for the animation. This can take any of the following values. The default mode is 'linear', however if the attribute does not support linear interpolation (e.g. for strings), the [‘calcMode’](#CalcModeAttribute) attribute is ignored and discrete interpolation is used.

**discrete**
: This specifies that the animation function will jump from one value to the next without any interpolation.
**linear**
: Simple linear interpolation between values is used to calculate the animation function. Except for [‘animateMotion’](#AnimateMotionElement), this is the default [‘calcMode’](#CalcModeAttribute).
**paced**
: Defines interpolation to produce an even pace of change across the animation. This is only supported for the data types for which there is an appropriate distance function defined, which includes only scalar numeric types plus the types listed in [Paced animation and complex types](#complexDistances). If 'paced' is specified, any [‘keyTimes’](#KeyTimesAttribute) or [‘keySplines’](#KeySplinesAttribute) will be ignored. For [‘animateMotion’](#AnimateMotionElement), this is the default [‘calcMode’](#CalcModeAttribute). Authors are discouraged from using paced animation on types that do not have a distance function defined, due to its unpredictable behavior in some user agents.
**spline**
: Interpolates from one value in the [‘values’](#ValuesAttribute) list to the next according to a time function defined by a cubic Bézier spline. The points of the spline are defined in the [‘keyTimes’](#KeyTimesAttribute) attribute, and the control points for each interval are defined in the [‘keySplines’](#KeySplinesAttribute) attribute.
Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'calcMode' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.3).

**<a id="ValuesAttribute"></a>values = "<list>"**
: A semicolon-separated list of one or more values. Vector-valued attributes are supported using the vector syntax of the [‘attributeType’](#AttributeTypeAttribute) domain. Per the SMIL specification, leading and trailing white space, and white space before and after semicolon separators, is allowed and will be ignored. Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'values' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.2).
**<a id="KeyTimesAttribute"></a>keyTimes = "<list>"**
:

A semicolon-separated list of time values used to control the pacing of the animation. Each time in the list corresponds to a value in the [‘values’](#ValuesAttribute) attribute list, and defines when the value is used in the animation function. Each time value in the [‘keyTimes’](#KeyTimesAttribute) list is specified as a floating point value between 0 and 1 (inclusive), representing a proportional offset into the simple duration of the animation element.

For animations specified with a [‘values’](#ValuesAttribute) list, the [‘keyTimes’](#KeyTimesAttribute) attribute if specified must have exactly as many values as there are in the [‘values’](#ValuesAttribute) attribute. For from/to/by animations, the [‘keyTimes’](#KeyTimesAttribute) attribute if specified must have two values.

Each successive time value must be greater than or equal to the preceding time value.

The [‘keyTimes’](#KeyTimesAttribute) list semantics depends upon the interpolation mode:

- For linear and spline animation, the first time value in the list must be 0, and the last time value in the list must be 1. The key time associated with each value defines when the value is set; values are interpolated between the key times.
- For discrete animation, the first time value in the list must be 0. The time associated with each value defines when the value is set; the animation function uses that value until the next time defined in [‘keyTimes’](#KeyTimesAttribute).

If the interpolation mode is 'paced', the [‘keyTimes’](#KeyTimesAttribute) attribute is ignored.

If there are any errors in the [‘keyTimes’](#KeyTimesAttribute) specification (bad values, too many or too few values), the document fragment is in error (see [error processing](implnote.md#ErrorProcessing)).

If the simple duration is indefinite, any [‘keyTimes’](#KeyTimesAttribute) specification will be ignored.

Because paced animation interpolation is unspecified for some value types, authors are encouraged to use 'linear' animation interpolation with calculated [‘keyTimes’](#KeyTimesAttribute) to achieve particular interpolation behavior for these types.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'keyTimes' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.3).

**<a id="KeySplinesAttribute"></a>keySplines = "<list>"**
:

A set of Bézier control points associated with the [‘keyTimes’](#KeyTimesAttribute) list, defining a cubic Bézier function that controls interval pacing. The attribute value is a semicolon-separated list of control point descriptions. Each control point description is a set of four values: `x1 y1 x2 y2`, describing the Bézier control points for one time segment. Note: SMIL allows these values to be separated either by commas with optional whitespace, or by whitespace alone. The [‘keyTimes’](#KeyTimesAttribute) values that define the associated segment are the Bézier "anchor points", and the [‘keySplines’](#KeySplinesAttribute) values are the control points. Thus, there must be one fewer sets of control points than there are [‘keyTimes’](#KeyTimesAttribute).

The values must all be in the range 0 to 1.

This attribute is ignored unless the [‘calcMode’](#CalcModeAttribute) is set to 'spline'.

If there are any errors in the [‘keySplines’](#KeySplinesAttribute) specification (bad values, too many or too few values), the document fragment is in error (see [error processing](implnote.md#ErrorProcessing)).

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'keySplines' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.3).

**<a id="FromAttribute"></a>from = "<value>"**
: Specifies the starting value of the animation. Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'from' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.2).
**<a id="ToAttribute"></a>to = "<value>"**
: Specifies the ending value of the animation. Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'to' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.2).
**<a id="ByAttribute"></a>by = "<value>"**
: Specifies a relative offset value for the animation. Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'by' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.2).
The SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)] defines the detailed processing rules associated with the above attributes. Except for any SVG-specific rules explicitly mentioned in this specification, the SMIL Animation specification is the normative definition of the processing rules for the above attributes.

The animation values specified in the animation element must be legal values for the specified attribute. Leading and trailing white space, and white space before and after semicolon separators, will be ignored.

All values specified must be legal values for the specified attribute (as defined in the associated namespace). If any values are not legal, the document fragment is in error (see [error processing](implnote.md#ErrorProcessing)).

If a list of values is used, the animation will apply the values in order over the course of the animation. If a list of [‘values’](#ValuesAttribute) is specified, any [‘from’](#FromAttribute), [‘to’](#ToAttribute) and [‘by’](#ByAttribute) attribute values are ignored.

The processing rules for the variants of *from/by/to* animations are described in Animation function values with the following exception.

In order to provide behavior that is intuitive and consistent between discrete animations with an explicitly specified [‘from’](#FromAttribute) attribute (e.g. "from-to animation") and those where the underlying value is used (e.g. "to animation"), the behavior of discrete to-animation in SVG deviates from the definition in SMIL Animation. As with a discrete from-to animation, a discrete to animation will set the underlying value for the first half of the simple duration (or, if a [‘keyTimes’](#KeyTimesAttribute) list is provided, until the simple duration specified by the second value in the [‘keyTimes’](#KeyTimesAttribute) list) and the [‘to’](#ToAttribute) value for the remainder of the simple duration.

The following figure illustrates the interpretation of the [‘keySplines’](#KeySplinesAttribute) attribute. Each diagram illustrates the effect of [‘keySplines’](#KeySplinesAttribute) settings for a single interval (i.e. between the associated pairs of values in the [‘keyTimes’](#KeyTimesAttribute) and [‘values’](#ValuesAttribute) lists.). The horizontal axis can be thought of as the input value for the *unit progress* of interpolation within the interval - i.e. the pace with which interpolation proceeds along the given interval. The vertical axis is the resulting value for the *unit progress*, yielded by the function that the [‘keySplines’](#KeySplinesAttribute) attribute defines. Another way of describing this is that the horizontal axis is the input *unit time* for the interval, and the vertical axis is the output *unit time*. See also the section Timing and real-world clock times.

Examples of keySplines

|  | [image: Example keySplines01 - keySplines of 0 0 1 1 (the default)] keySplines="0 0 1 1" (the default) |  | [image: Example keySplines01 - keySplines of .5 0 .5 1] keySplines=".5 0 .5 1" |
| --- | --- | --- | --- |
|  | [image: Example keySplines01 - keySplines of 0 .75 .25 1] keySplines="0 .75 .25 1" |  | [image: Example keySplines01 - keySplines of 1 0 .25 .25] keySplines="1 0 .25 .25" |

To illustrate the calculations, consider the simple example:

```
<animate dur="4s" values="10; 20" keyTimes="0; 1"
     calcMode="spline" keySplines={as in table} />
```

Using the [‘keySplines’](#KeySplinesAttribute) values for each of the four cases above, the approximate interpolated values as the animation proceeds are:

| Value of ‘keySplines’ | Initial value | After 1s | After 2s | After 3s | Final value |
| --- | --- | --- | --- | --- | --- |
| 0 0 1 1 | 10.0 | 12.5 | 15.0 | 17.5 | 20.0 |
| .5 0 .5 1 | 10.0 | 11.0 | 15.0 | 19.0 | 20.0 |
| 0 .75 .25 1 | 10.0 | 18.0 | 19.3 | 19.8 | 20.0 |
| 1 0 .25 .25 | 10.0 | 10.1 | 10.6 | 16.9 | 20.0 |

For a formal definition of Bézier spline calculation, see [[FOLEY-VANDAM](refs.md#ref-FOLEY-VANDAM)], pp. 488-491.

### <a id="AdditionAttributes"></a>19.2.10 Attributes that control whether animations are additive

It is frequently useful to define animation as an offset or delta to an attribute's value, rather than as absolute values. A simple "grow" animation can increase the width of an object by 10 pixels:

```
<rect width="20px" ...>
  <animate attributeName="width" from="0px" to="10px" dur="10s"
           additive="sum"/>
</rect>
```

It is frequently useful for repeated animations to build upon the previous results, accumulating with each interation. The following example causes the rectangle to continue to grow with each repeat of the animation:

```
<rect width="20px" ...>
  <animate attributeName="width" from="0px" to="10px" dur="10s"
           additive="sum" accumulate="sum" repeatCount="5"/>
</rect>
```

At the end of the first repetition, the rectangle has a width of 30 pixels. At the end of the second repetition, the rectangle has a width of 40 pixels. At the end of the fifth repetition, the rectangle has a width of 70 pixels.

For more information about additive animations, see SMIL Animation: Additive animation. For more information on cumulative animations, see SMIL Animation: Controlling behavior of repeating animation - Cumulative animation.

The following attributes are the animation addition attributes, which are common to elements [‘animate’](#AnimateElement), [‘animateColor’](#AnimateColorElement), [‘animateMotion’](#AnimateMotionElement) and [‘animateTransform’](#AnimateTransformElement).

*Attribute definitions:*

**<a id="AdditiveAttribute"></a>additive = "replace | sum"**
:

Controls whether or not the animation is additive.

**sum**
: Specifies that the animation will add to the underlying value of the attribute and other lower priority animations.
**replace**
: Specifies that the animation will override the underlying value of the attribute and other lower priority animations. This is the default, however the behavior is also affected by the animation value attributes [‘by’](#ByAttribute) and [‘to’](#ToAttribute), as described in SMIL Animation: How from, to and by attributes affect additive behavior.
Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'additive' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.6).

**<a id="AccumulateAttribute"></a>accumulate = "none | sum"**
:

Controls whether or not the animation is cumulative.

**sum**
: Specifies that each repeat iteration after the first builds upon the last value of the previous iteration.
**none**
: Specifies that repeat iterations are not cumulative. This is the default.
This attribute is ignored if the target attribute value does not support addition, or if the animation element does not repeat.

Cumulative animation is not defined for "*to animation*".

This attribute will be ignored if the animation function is specified with only the [‘to’](#ToAttribute) attribute.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this attribute is the SMIL Animation specification. In particular, see SMIL Animation: 'accumulate' attribute ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.1).

### <a id="Inheritance"></a>19.2.11 Inheritance

SVG allows both attributes and properties to be animated. If a given attribute or property is inheritable by descendants, then animations on a parent element such as a [‘g’](struct.md#GElement) element has the effect of propagating the attribute or property animation values to descendant elements as the animation proceeds; thus, descendant elements can inherit animated attributes and properties from their ancestors.

### <a id="AnimateElement"></a>19.2.12 The ‘animate’ element

The [‘animate’](#AnimateElement) element is used to animate a single attribute or property over time. For example, to make a rectangle repeatedly fade away over 5 seconds, you can specify:

```
<rect>
  <animate attributeType="CSS" attributeName="opacity" 
         from="1" to="0" dur="5s" repeatCount="indefinite" />
</rect>
```

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this element is the SMIL Animation specification. In particular, see SMIL Animation: 'animate' element ([[SMILANIM](refs.md#ref-SMILANIM)], section 4.1).

‘animate’

**Categories:**
: [Animation element](intro.md#TermAnimationElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [animation event attributes](intro.md#TermAnimationEventAttribute) — [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#HrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [animation attribute target attributes](#TargetAttributes) — [‘attributeType’](#AttributeTypeAttribute), [‘attributeName’](#AttributeNameAttribute)
- [animation timing attributes](#TimingAttributes) — [‘begin’](#BeginAttribute), [‘dur’](#DurAttribute), [‘end’](#EndAttribute), [‘min’](#MinAttribute), [‘max’](#MaxAttribute), [‘restart’](#RestartAttribute), [‘repeatCount’](#RepeatCountAttribute), [‘repeatDur’](#RepeatDurAttribute), [‘fill’](#FillAttribute)
- [animation value attributes](#ValueAttributes) — [‘calcMode’](#CalcModeAttribute), [‘values’](#ValuesAttribute), [‘keyTimes’](#KeyTimesAttribute), [‘keySplines’](#KeySplinesAttribute), [‘from’](#FromAttribute), [‘to’](#ToAttribute), [‘by’](#ByAttribute)
- [animation addition attributes](#AdditionAttributes) — [‘additive’](#AdditiveAttribute), [‘accumulate’](#AccumulateAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)

**DOM Interfaces:**
:

- [SVGAnimateElement](#InterfaceSVGAnimateElement)

The [‘color-interpolation’](painting.md#ColorInterpolationProperty) property applies to color interpolations that result from animations using the [‘animate’](#AnimateElement) element.

For a list of attributes and properties that can be animated using the [‘animate’](#AnimateElement) element, see [Elements, attributes and properties that can be animated](#AnimationAttributesAndProperties).

### <a id="SetElement"></a>19.2.13 The ‘set’ element

The [‘set’](#SetElement) element provides a simple means of just setting the value of an attribute for a specified duration. It supports all attribute types, including those that cannot reasonably be interpolated, such as string and boolean values. The [‘set’](#SetElement) element is non-additive. The additive and accumulate attributes are not allowed, and will be ignored if specified.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this element is the SMIL Animation specification. In particular, see SMIL Animation: 'set' element ([[SMILANIM](refs.md#ref-SMILANIM)], section 4.2).

‘set’

**Categories:**
: [Animation element](intro.md#TermAnimationElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [animation event attributes](intro.md#TermAnimationEventAttribute) — [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#HrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [animation attribute target attributes](#TargetAttributes) — [‘attributeType’](#AttributeTypeAttribute), [‘attributeName’](#AttributeNameAttribute)
- [animation timing attributes](#TimingAttributes) — [‘begin’](#BeginAttribute), [‘dur’](#DurAttribute), [‘end’](#EndAttribute), [‘min’](#MinAttribute), [‘max’](#MaxAttribute), [‘restart’](#RestartAttribute), [‘repeatCount’](#RepeatCountAttribute), [‘repeatDur’](#RepeatDurAttribute), [‘fill’](#FillAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘to’](#SetElementToAttribute)

**DOM Interfaces:**
:

- [SVGSetElement](#InterfaceSVGSetElement)

*Attribute definitions:*

**<a id="SetElementToAttribute"></a>to = "<value>"**
: Specifies the value for the attribute during the duration of the [‘set’](#SetElement) element. The argument value must match the attribute type.
For a list of attributes and properties that can be animated using the [‘set’](#SetElement) element, see [Elements, attributes and properties that can be animated](#AnimationAttributesAndProperties).

### <a id="AnimateMotionElement"></a>19.2.14 The ‘animateMotion’ element

The [‘animateMotion’](#AnimateMotionElement) element causes a referenced element to move along a motion path.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this element is the SMIL Animation specification. In particular, see SMIL Animation: 'animateMotion' element ([[SMILANIM](refs.md#ref-SMILANIM)], section 4.3).

‘animateMotion’

**Categories:**
: [Animation element](intro.md#TermAnimationElement)
**Content model:**
: Any number of [descriptive elements](intro.md#TermDescriptiveElement) and at most one [‘mpath’](#MPathElement) element, in any order.
**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [animation event attributes](intro.md#TermAnimationEventAttribute) — [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#HrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [animation timing attributes](#TimingAttributes) — [‘begin’](#BeginAttribute), [‘dur’](#DurAttribute), [‘end’](#EndAttribute), [‘min’](#MinAttribute), [‘max’](#MaxAttribute), [‘restart’](#RestartAttribute), [‘repeatCount’](#RepeatCountAttribute), [‘repeatDur’](#RepeatDurAttribute), [‘fill’](#FillAttribute)
- [animation value attributes](#ValueAttributes) — [‘calcMode’](#CalcModeAttribute), [‘values’](#ValuesAttribute), [‘keyTimes’](#KeyTimesAttribute), [‘keySplines’](#KeySplinesAttribute), [‘from’](#FromAttribute), [‘to’](#ToAttribute), [‘by’](#ByAttribute)
- [animation addition attributes](#AdditionAttributes) — [‘additive’](#AdditiveAttribute), [‘accumulate’](#AccumulateAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘path’](#PathAttribute)
- [‘keyPoints’](#KeyPointsAttribute)
- [‘rotate’](#RotateAttribute)
- [‘origin’](#OriginAttribute)

**DOM Interfaces:**
:

- [SVGAnimateMotionElement](#InterfaceSVGAnimateMotionElement)

*Attribute definitions:*

**<a id="AnimateMotionElementCalcModeAttribute"></a>calcMode = "discrete | linear | paced | spline"**
: Specifies the interpolation mode for the animation. Refer to general description of the [‘calcMode’](#CalcModeAttribute) attribute above. The only difference is that the default value for the [‘calcMode’](#CalcModeAttribute) for [‘animateMotion’](#AnimateMotionElement) is 'paced'. See SMIL Animation: 'calcMode' attribute for 'animateMotion'.
**<a id="PathAttribute"></a>path = "<path-data>"**
: The motion path, expressed in the same format and interpreted the same way as the [‘d’](paths.md#DAttribute) attribute on the [‘path’](paths.md#PathElement) element. The effect of a motion path animation is to add a supplemental transformation matrix onto the CTM for the referenced object which causes a translation along the x- and y-axes of the current user coordinate system by the computed X and Y values computed over time.
**<a id="KeyPointsAttribute"></a>keyPoints = "<list-of-numbers>"**
:

[‘keyPoints’](#KeyPointsAttribute) takes a semicolon-separated list of floating point values between 0 and 1 and indicates how far along the motion path the object shall move at the moment in time specified by corresponding [‘keyTimes’](#KeyTimesAttribute) value. Distance calculations use the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. Each progress value in the list corresponds to a value in the [‘keyTimes’](#KeyTimesAttribute) attribute list.

If a list of [‘keyPoints’](#KeyPointsAttribute) is specified, there must be exactly as many values in the [‘keyPoints’](#KeyPointsAttribute) list as in the [‘keyTimes’](#KeyTimesAttribute) list.

If there are any errors in the [‘keyPoints’](#KeyPointsAttribute) specification (bad values, too many or too few values), then the document is in error (see [Error processing](implnote.md#ErrorProcessing)).

**<a id="RotateAttribute"></a>rotate = "[<number>](types.md#DataTypeNumber) | auto | auto-reverse"**
:

The [‘rotate’](#RotateAttribute) attribute post-multiplies a supplemental transformation matrix onto the CTM of the target element to apply a rotation transformation about the origin of the current user coordinate system. The rotation transformation is applied after the supplemental translation transformation that is computed due to the [‘path’](#PathAttribute) attribute.

**auto**
: Indicates that the object is rotated over time by the angle of the direction (i.e., directional tangent vector) of the motion path.
**auto-reverse**
: Indicates that the object is rotated over time by the angle of the direction (i.e., directional tangent vector) of the motion path plus 180 degrees.
**[<number>](types.md#DataTypeNumber)**
: Indicates that the target element has a constant rotation transformation applied to it, where the rotation angle is the specified number of degrees.
The default value is '0'.

**<a id="OriginAttribute"></a>origin = "default"**
: The [‘origin’](#OriginAttribute) attribute is defined in the SMIL Animation specification ([[SMILANIM](refs.md#ref-SMILANIM)], section 4.3). It has no effect in SVG.
<a id="MPathElement"></a>

‘mpath’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#MPathElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘xlink:href’](#MPathElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGMPathElement](#InterfaceSVGMPathElement)

*Attribute definitions:*

**<a id="MPathElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](intro.md#TermIRIReference) to the [‘path’](paths.md#PathElement) element which defines the motion path. [Animatable](#Animatable): no.
For [‘animateMotion’](#AnimateMotionElement), the specified values for [‘from’](#FromAttribute), [‘by’](#ByAttribute), [‘to’](#ToAttribute) and [‘values’](#ValuesAttribute) consists of x, y coordinate pairs, with a single comma and/or white space separating the x coordinate from the y coordinate. For example, from="33,15" specifies an x coordinate value of 33 and a y coordinate value of 15.

If provided, the [‘values’](#ValuesAttribute) attribute must consists of a list of x, y coordinate pairs. Coordinate values are separated by at least one white space character or a comma. Additional white space around the separator is allowed. For example, values="10,20;30,20;30,40" or values="10mm,20mm;30mm,20mm;30mm,40mm". Each coordinate represents a [length](types.md#DataTypeLength). Attributes [‘from’](#FromAttribute), [‘by’](#ByAttribute), [‘to’](#ToAttribute) and [‘values’](#ValuesAttribute) specify a shape on the current canvas which represents the motion path.

Two options are available which allow definition of a motion path using any of SVG's [path data](paths.md#PathData) commands:

- the [‘path’](#PathAttribute) attribute defines a motion path directly on [‘animateMotion’](#AnimateMotionElement) element using any of SVG's [path data](paths.md#PathData) commands.
- the [‘mpath’](#MPathElement) sub-element provides the ability to reference an external [‘path’](paths.md#PathElement) element as the definition of the motion path.

Note that SVG's [path data](paths.md#PathData) commands can only contain values in user space, whereas [‘from’](#FromAttribute), [‘by’](#ByAttribute), [‘to’](#ToAttribute) and [‘values’](#ValuesAttribute) can specify coordinates in user space or using unit identifiers. See [Units](coords.md#Units).

The various (x,y) points of the shape provide a supplemental transformation matrix onto the CTM for the referenced object which causes a translation along the x- and y-axes of the current user coordinate system by the (x,y) values of the shape computed over time. Thus, the referenced object is translated over time by the offset of the motion path relative to the origin of the current user coordinate system. The supplemental transformation is applied on top of any transformations due to the target element's [‘transform’](coords.md#TransformAttribute) attribute or any animations on that attribute due to [‘animateTransform’](#AnimateTransformElement) elements on the target element.

The [‘additive’](#AdditiveAttribute) and [‘accumulate’](#AccumulateAttribute) attributes apply to [‘animateMotion’](#AnimateMotionElement) elements. Multiple [‘animateMotion’](#AnimateMotionElement) elements all simultaneously referencing the same target element can be additive with respect to each other; however, the transformations which result from the [‘animateMotion’](#AnimateMotionElement) elements are always supplemental to any transformations due to the target element's [‘transform’](coords.md#TransformAttribute) attribute or any [‘animateTransform’](#AnimateTransformElement) elements.

The default calculation mode ([‘calcMode’](#CalcModeAttribute)) for [‘animateMotion’](#AnimateMotionElement) is "paced". This will produce constant velocity motion along the specified path. Note that while animateMotion elements can be additive, it is important to observe that the addition of two or more "paced" (constant velocity) animations might not result in a combined motion animation with constant velocity.

When a path is combined with "discrete", "linear" or "spline" [‘calcMode’](#CalcModeAttribute) settings, and if attribute [‘keyPoints’](#KeyPointsAttribute) is not provided, the number of values is defined to be the number of points defined by the path, unless there are "move to" commands within the path. A "move to" command within the path (i.e. other than at the beginning of the path description) A "move to" command does not count as an additional point when dividing up the duration, or when associating [‘keyTimes’](#KeyTimesAttribute), [‘keySplines’](#KeySplinesAttribute) and [‘keyPoints’](#KeyPointsAttribute) values. When a path is combined with a "paced" [‘calcMode’](#CalcModeAttribute) setting, all "move to" commands are considered to have 0 length (i.e. they always happen instantaneously), and is not considered in computing the pacing.

For more flexibility in controlling the velocity along the motion path, the [‘keyPoints’](#KeyPointsAttribute) attribute provides the ability to specify the progress along the motion path for each of the [‘keyTimes’](#KeyTimesAttribute) specified values. If specified, [‘keyPoints’](#KeyPointsAttribute) causes [‘keyTimes’](#KeyTimesAttribute) to apply to the values in [‘keyPoints’](#KeyPointsAttribute) rather than the points specified in the [‘values’](#ValuesAttribute) attribute array or the points on the [‘path’](#PathAttribute) attribute.

The override rules for [‘animateMotion’](#AnimateMotionElement) are as follows. Regarding the definition of the motion path, the [‘mpath’](#MPathElement) element overrides the the [‘path’](#PathAttribute) attribute, which overrides [‘values’](#ValuesAttribute), which overrides [‘from’](#FromAttribute), [‘by’](#ByAttribute) and [‘to’](#ToAttribute). Regarding determining the points which correspond to the [‘keyTimes’](#KeyTimesAttribute) attributes, the [‘keyPoints’](#KeyPointsAttribute) attribute overrides [‘path’](#PathAttribute), which overrides [‘values’](#ValuesAttribute), which overrides [‘from’](#FromAttribute), [‘by’](#ByAttribute) and [‘to’](#ToAttribute).

At any time *t* within a motion path animation of duration *dur*, the computed coordinate (x,y) along the motion path is determined by finding the point (x,y) which is *t/dur* distance along the motion path using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm.

The following example demonstrates the supplemental transformation matrices that are computed during a motion path animation.

<a id="ExampleAnimMotion01"></a>

Example animMotion01 shows a triangle moving along a motion path.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="5cm" height="3cm"  viewBox="0 0 500 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1"
     xmlns:xlink="http://www.w3.org/1999/xlink" >
  <desc>Example animMotion01 - demonstrate motion animation computations</desc>
  <rect x="1" y="1" width="498" height="298"
        fill="none" stroke="blue" stroke-width="2" />
  <!-- Draw the outline of the motion path in blue, along
          with three small circles at the start, middle and end. -->
  <path id="path1" d="M100,250 C 100,50 400,50 400,250"
        fill="none" stroke="blue" stroke-width="7.06"  />
  <circle cx="100" cy="250" r="17.64" fill="blue"  />
  <circle cx="250" cy="100" r="17.64" fill="blue"  />
  <circle cx="400" cy="250" r="17.64" fill="blue"  />
  <!-- Here is a triangle which will be moved about the motion path.
       It is defined with an upright orientation with the base of
       the triangle centered horizontally just above the origin. -->
  <path d="M-25,-12.5 L25,-12.5 L 0,-87.5 z"
        fill="yellow" stroke="red" stroke-width="7.06"  >
    <!-- Define the motion path animation -->
    <animateMotion dur="6s" repeatCount="indefinite" rotate="auto" >
       <mpath xlink:href="#path1"/>
    </animateMotion>
  </path>
</svg>
```

Example animMotion01

| [image: Example animMotion01 - at zero seconds] At zero seconds |  | [image: Example animMotion01 - at three seconds] At three seconds |  | [image: Example animMotion01 - at six seconds] At six seconds |
| --- | --- | --- | --- | --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/animate/animMotion01.svg)

The following table shows the supplemental transformation matrices that are applied to achieve the effect of the motion path animation.

|  | After 0s | After 3s | After 6s |
| --- | --- | --- | --- |
| Supplemental transform due to movement along motion path | translate(100,250) | translate(250,100) | translate(400,250) |
| Supplemental transform due to rotate="auto" | rotate(-90) | rotate(0) | rotate(90) |

For a list of elements that can be animated using the [‘animateMotion’](#AnimateMotionElement) element, see [Elements, attributes and properties that can be animated](#AnimationAttributesAndProperties).

### <a id="AnimateColorElement"></a>19.2.15 The ‘animateColor’ element

The [‘animateColor’](#AnimateColorElement) element specifies a color transformation over time.

Except for any SVG-specific rules explicitly mentioned in this specification, the normative definition for this element is the SMIL Animation specification. In particular, see SMIL Animation: 'animateColor' element ([[SMILANIM](refs.md#ref-SMILANIM)], section 4.4).

‘animateColor’

**Categories:**
: [Animation element](intro.md#TermAnimationElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [animation event attributes](intro.md#TermAnimationEventAttribute) — [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#HrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [animation attribute target attributes](#TargetAttributes) — [‘attributeType’](#AttributeTypeAttribute), [‘attributeName’](#AttributeNameAttribute)
- [animation timing attributes](#TimingAttributes) — [‘begin’](#BeginAttribute), [‘dur’](#DurAttribute), [‘end’](#EndAttribute), [‘min’](#MinAttribute), [‘max’](#MaxAttribute), [‘restart’](#RestartAttribute), [‘repeatCount’](#RepeatCountAttribute), [‘repeatDur’](#RepeatDurAttribute), [‘fill’](#FillAttribute)
- [animation value attributes](#ValueAttributes) — [‘calcMode’](#CalcModeAttribute), [‘values’](#ValuesAttribute), [‘keyTimes’](#KeyTimesAttribute), [‘keySplines’](#KeySplinesAttribute), [‘from’](#FromAttribute), [‘to’](#ToAttribute), [‘by’](#ByAttribute)
- [animation addition attributes](#AdditionAttributes) — [‘additive’](#AdditiveAttribute), [‘accumulate’](#AccumulateAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)

**DOM Interfaces:**
:

- [SVGAnimateColorElement](#InterfaceSVGAnimateColorElement)

The [‘from’](#FromAttribute), [‘by’](#ByAttribute) and [‘to’](#ToAttribute) attributes take color values, where each color value is expressed using the following syntax (the same syntax as used in SVG's properties that can take color values):

```
<color> <icccolor>?
```

The [‘values’](#ValuesAttribute) attribute for the [‘animateColor’](#AnimateColorElement) element consists of a semicolon-separated list of color values, with each color value expressed in the above syntax.

Out of range color values can be provided, but user agent processing will be implementation dependent. User agents should clamp color values to allow color range values as late as possible, but note that system differences might preclude consistent behavior across different systems.

The [‘color-interpolation’](painting.md#ColorInterpolationProperty) property applies to color interpolations that result from [‘animateColor’](#AnimateColorElement) animations.

The use of [‘animateColor’](#AnimateColorElement) is deprecated, since all of its functionality can be achieved simply by using [‘animate’](#AnimateElement) to target properties that can take color values. The [‘animateColor’](#AnimateColorElement) element may be dropped from a future version of the SVG specification.

For a list of attributes and properties that can be animated using the [‘animateColor’](#AnimateColorElement) element, see [Elements, attributes and properties that can be animated](#AnimationAttributesAndProperties).

### <a id="AnimateTransformElement"></a>19.2.16 The ‘animateTransform’ element

The [‘animateTransform’](#AnimateTransformElement) element animates a transformation attribute on a target element, thereby allowing animations to control translation, scaling, rotation and/or skewing.

‘animateTransform’

**Categories:**
: [Animation element](intro.md#TermAnimationElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [animation event attributes](intro.md#TermAnimationEventAttribute) — [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#HrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [animation attribute target attributes](#TargetAttributes) — [‘attributeType’](#AttributeTypeAttribute), [‘attributeName’](#AttributeNameAttribute)
- [animation timing attributes](#TimingAttributes) — [‘begin’](#BeginAttribute), [‘dur’](#DurAttribute), [‘end’](#EndAttribute), [‘min’](#MinAttribute), [‘max’](#MaxAttribute), [‘restart’](#RestartAttribute), [‘repeatCount’](#RepeatCountAttribute), [‘repeatDur’](#RepeatDurAttribute), [‘fill’](#FillAttribute)
- [animation value attributes](#ValueAttributes) — [‘calcMode’](#CalcModeAttribute), [‘values’](#ValuesAttribute), [‘keyTimes’](#KeyTimesAttribute), [‘keySplines’](#KeySplinesAttribute), [‘from’](#FromAttribute), [‘to’](#ToAttribute), [‘by’](#ByAttribute)
- [animation addition attributes](#AdditionAttributes) — [‘additive’](#AdditiveAttribute), [‘accumulate’](#AccumulateAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘type’](#AnimateTransformElementTypeAttribute)

**DOM Interfaces:**
:

- [SVGAnimateTransformElement](#InterfaceSVGAnimateTransformElement)

*Attribute definitions:*

**<a id="AnimateTransformElementTypeAttribute"></a>type = "translate | scale | rotate | skewX | skewY"**
: Indicates the type of transformation which is to have its values change over time. If the attribute is not specified, then the effect is as if a value of 'translate' were specified.
The [‘from’](#FromAttribute), [‘by’](#ByAttribute) and [‘to’](#ToAttribute) attributes take a value expressed using the same syntax that is available for the given transformation type:

- For a type="translate", each individual value is expressed as <tx> [,<ty>].
- For a type="scale", each individual value is expressed as <sx> [,<sy>].
- For a type="rotate", each individual value is expressed as <rotate-angle> [<cx> <cy>].
- For a type="skewX" and type="skewY", each individual value is expressed as <skew-angle>.

(See [The ‘transform’ attribute](coords.md#TransformAttribute).)

The [‘values’](#ValuesAttribute) attribute for the [‘animateTransform’](#AnimateTransformElement) element consists of a semicolon-separated list of values, where each individual value is expressed as described above for [‘from’](#FromAttribute), [‘by’](#ByAttribute) and [‘to’](#ToAttribute).

The animation effect for [‘animateTransform’](#AnimateTransformElement) is post-multiplied to the underlying value for additive [‘animateTransform’](#AnimateTransformElement) animations (see below) instead of added to the underlying value, due to the specific behavior of [‘animateTransform’](#AnimateTransformElement).

*From-to*, *from-by* and *by animations* are defined in SMIL to be equivalent to a corresponding *values animation*. See the Animation function values section of SMIL Animation ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.2.2). However, *to animations* are a mixture of additive and non-additive behavior, as described in the How from, to and by attributes affect additive behavior section of SMIL Animation ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.6). *To animations* provide specific functionality to get a smooth change from the underlying value to the [‘to’](#ToAttribute) attribute value, which conflicts mathematically with the requirement for additive transform animations to be post-multiplied. As a consequence, in SVG 1.1 the behavior of *to animations* for [‘animateTransform’](#AnimateTransformElement) is undefined. Authors are suggested to use *from-to*, *from-by*, *by* or *values animations* to achieve any desired transform animation.

If [‘calcMode’](#CalcModeAttribute) has the value 'paced', then the "distance" for the transformation is calculated as further described in [Paced animations and complex types](#complexDistances).

When an animation is active, the effect of non-additive [‘animateTransform’](#AnimateTransformElement) (i.e., additive="replace") is to replace the given attribute's value with the transformation defined by the [‘animateTransform’](#AnimateTransformElement). The effect of additive (i.e., additive="sum") is to post-multiply the transformation matrix corresponding to the transformation defined by this [‘animateTransform’](#AnimateTransformElement). To illustrate:

```
<rect transform="skewX(30)"...>
  <animateTransform attributeName="transform" attributeType="XML"
                    type="rotate" from="0" to="90" dur="5s"
                    additive="replace" fill="freeze"/>
  <animateTransform attributeName="transform" attributeType="XML"
                    type="scale" from="1" to="2" dur="5s"
                    additive="replace" fill="freeze"/>
</rect>
```

In the code snippet above, because the both animations have additive="replace", the first animation overrides the transformation on the rectangle itself and the second animation overrides the transformation from the first animation; therefore, at time 5 seconds, the visual result of the above two animations would be equivalent to the following static rectangle:

```
<rect transform="scale(2)" ... />
```

whereas in the following example:

```
<rect transform="skewX(30)"...>
  <animateTransform attributeName="transform" attributeType="XML"
                    type="rotate" from="0" to="90" dur="5s" 
                    additive="sum" fill="freeze"/>
  <animateTransform attributeName="transform" attributeType="XML"
                    type="scale" from="1" to="2" dur="5s"
                    additive="sum" fill="freeze"/>
</rect>
```

In this code snippet, because the both animations have additive="sum", the first animation post-multiplies its transformation to any transformations on the rectangle itself and the second animation post-multiplies its transformation to any transformation from the first animation; therefore, at time 5 seconds, the visual result of the above two animations would be equivalent to the following static rectangle:

```
<rect transform="skewX(30) rotate(90) scale(2)" ... />
```

Note that the zero value used when performing a *by animation* with type="scale" is indeed 0. Thus, performing the following animation causes the rectangle to be invisible at time 0s (since the animated transform list value is 'scale(0)'), and be scaled back to its original size at time 5s (since the animated transform list value is 'scale(1)'):

```
<rect width="100" height="100">
  <animateTransform attributeName="transform" attributeType="XML"
                    type="scale" by="1" dur="5s" fill="freeze"/>
</rect>
```

When a transform animation has accumulate='sum', the accumulation that occurs for each completed repetition of the animation is computed on the values specified in the [‘animateTransform’](#AnimateTransformElement) element's [animation value attributes](#ValueAttributes) (i.e., [‘values’](#ValuesAttribute), [‘from’](#FromAttribute), [‘to’](#ToAttribute) and [‘by’](#ByAttribute)) and not on the transformation matrix that these values represent. For example, in the following code snippet, 3 is added to the scale value at the start of each repetition:

```
<rect width="100" height="100">
  <animateTransform attributeName="transform" attributeType="XML"
                    type="scale" from="2" to="3" repeatCount="3" dur="4s"
                    fill="freeze"/>
</rect>
```

The following graph and table shows the animated [‘transform’](coords.md#TransformAttribute) value on the [‘rect’](shapes.md#RectElement) over the course of the animation:

| [image: The scale value animates from 2 to 12 with discontinuities at 4s and 8s.] |  |
| --- | --- |
| Time | Value |
| 0s | scale(2) |
| 1s | scale(2.25) |
| 2s | scale(2.5) |
| 3s | scale(2.75) |
| 4s | scale(5) |
| 5s | scale(5.25) |
| 6s | scale(5.5) |
| 7s | scale(5.75) |
| 8s | scale(8) |
| 9s | scale(8.25) |
| 10s | scale(8.5) |
| 11s | scale(8.75) |
| 12s | scale(9) |

Transform item types that can have multiple values – 'translate', 'scale' and 'rotate' – are treated as vectors and accumulation is performed with vector addition. Optional values that are omitted are taken to have their usual implied value: 1 for the <sy> component of a 'scale' and 0 for the <tx> component of a 'translate' and the <cx cy> components of a 'rotate'.

For example, consider the following code snippet, which has a cumulative transform animation of type 'rotate':

```
<rect width="100" height="100">
  <animateTransform attributeName="transform" attributeType="XML"
                    type="rotate" from="0 30 40" to="10 30 40"
                    repeatCount="2" dur="1s" fill="freeze"/>
</rect>
```

At time 1 second, the animated value of [‘transform’](coords.md#TransformAttribute) on the [‘rect’](shapes.md#RectElement) will jump from 'rotate(10 30 40)' to 'rotate(10 60 80)', because the effect of the accumulation is to take the value at the end of the first repetition, '10 30 40', and add to it the value at simple duration *t* = 0s, which is '0 30 40'.

For a list of attributes and properties that can be animated using the [‘animateTransform’](#AnimateTransformElement) element, see [Elements, attributes and properties that can be animated](#AnimationAttributesAndProperties).

### <a id="AnimationAttributesAndProperties"></a>19.2.17 Elements, attributes and properties that can be animated

The following lists all of the elements which can be animated by an [‘animateMotion’](#AnimateMotionElement) element:

- [‘svg’](struct.md#SVGElement)
- [‘g’](struct.md#GElement)
- [‘defs’](struct.md#DefsElement)
- [‘use’](struct.md#UseElement)
- [‘image’](struct.md#ImageElement)
- [‘switch’](struct.md#SwitchElement)
- [‘path’](paths.md#PathElement)
- [‘rect’](shapes.md#RectElement)
- [‘circle’](shapes.md#CircleElement)
- [‘ellipse’](shapes.md#EllipseElement)
- [‘line’](shapes.md#LineElement)
- [‘polyline’](shapes.md#PolylineElement)
- [‘polygon’](shapes.md#PolygonElement)
- [‘text’](text.md#TextElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘mask’](masking.md#MaskElement)
- [‘a’](linking.md#AElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)

<a id="Animatable"></a>Each attribute or property within this specification indicates whether or not it can be animated by SVG's animation elements. Animatable attributes and properties are designated as follows:

Animatable: yes.

whereas attributes and properties that cannot be animated are designated:

Animatable: no.

Some properties are defined as being animatable but only for non-additive animations:

Animatable: yes (non-additive).

SVG has a defined set of [basic data types](types.md#BasicDataTypes) for its various supported attributes and properties. For those attributes and properties that can be animated, the following table indicates which animation elements can be used to animate each of the basic data types. If a given attribute or property can take values of keywords (which are not additive) or numeric values (which are additive), then additive animations are possible if the subsequent animation uses a numeric value even if the base animation uses a keyword value; however, if the subsequent animation uses a keyword value, additive animation is not possible.

| Data type | Additive? | [‘animate’](#AnimateElement) | [‘set’](#SetElement) | [‘animateColor’](#AnimateColorElement) | [‘animateTransform’](#AnimateTransformElement) | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| [<angle>](types.md#DataTypeAngle) | yes | yes | yes | no | no |  |
| [<color>](types.md#DataTypeColor) | yes | yes | yes | yes | no | Only additive if each value can be converted to an RGB color. |
| [<coordinate>](types.md#DataTypeCoordinate) | yes | yes | yes | no | no |  |
| [<frequency>](types.md#DataTypeFrequency) | no | no | no | no | no |  |
| [<integer>](types.md#DataTypeInteger) | yes | yes | yes | no | no |  |
| [<length>](types.md#DataTypeLength) | yes | yes | yes | no | no |  |
| [<list-of-*T*s>](types.md#DataTypeList) | no | yes | yes | no | no |  |
| [<number>](types.md#DataTypeNumber) | yes | yes | yes | no | no |  |
| [<paint>](types.md#DataTypePaint) | yes | yes | yes | yes | no | Only additive if each value can be converted to an RGB color. |
| [<percentage>](types.md#DataTypePercentage) | yes | yes | yes | no | no |  |
| [<time>](types.md#DataTypeTime) | no | no | no | no | no |  |
| [<transform-list>](types.md#DataTypeTransformList) | yes | no | no | no | yes | Additive means that a transformation is post-multiplied to the base set of transformations. |
| [<iri>](types.md#DataTypeIRI) | no | yes | yes | no | no |  |
| All other data types used in animatable attributes and properties | no | yes | yes | no | no |  |

Any deviation from the above table or other special note about the animation capabilities of a particular attribute or property is included in the section of the specification where the given attribute or property is defined.

## <a id="DOMAnimationExample"></a>19.3 Animation using the SVG DOM

Example dom01 shows a simple animation using the DOM.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="4cm" height="2cm" viewBox="0 0 400 200"
     xmlns="http://www.w3.org/2000/svg"
     onload="StartAnimation(evt)" version="1.1">
  <script type="application/ecmascript"><![CDATA[
    var timevalue = 0;
    var timer_increment = 50;
    var max_time = 5000;
    var text_element;
    function StartAnimation(evt) {
      text_element = evt.target.ownerDocument.getElementById("TextElement");
      ShowAndGrowElement();
    }
    function ShowAndGrowElement() {
      timevalue = timevalue + timer_increment;
      if (timevalue > max_time)
        return;
      // Scale the text string gradually until it is 20 times larger
      scalefactor = (timevalue * 20.) / max_time;
      text_element.setAttribute("transform", "scale(" + scalefactor + ")");
      // Make the string more opaque
      opacityfactor = timevalue / max_time;
      text_element.setAttribute("opacity", opacityfactor);
      // Call ShowAndGrowElement again <timer_increment> milliseconds later.
      setTimeout("ShowAndGrowElement()", timer_increment)
    }
    window.ShowAndGrowElement = ShowAndGrowElement
  ]]></script>
  <rect x="1" y="1" width="398" height="198"
        fill="none" stroke="blue" stroke-width="2"/>
  <g transform="translate(50,150)" fill="red" font-size="7">
    <text id="TextElement">SVG</text>
  </g>
</svg>
```

Example dom01

| [image: Example dom01 - at zero seconds] At zero seconds |  | [image: Example dom01 - at three seconds] At 2.5 seconds |  | [image: Example dom01 - at six seconds] At five seconds |
| --- | --- | --- | --- | --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/animate/dom01.svg)

The above SVG file contains a single graphics element, a text string that says "SVG". The animation loops for 5 seconds. The text string starts out small and transparent and grows to be large and opaque. Here is an explanation of how this example works:

- The onload="StartAnimation(evt)" attribute indicates that, once the document has been fully loaded and processed, invoke ECMAScript function `StartAnimation`.
- The [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement) element defines the ECMAScript which makes the animation happen. The `StartAnimation()` function is only called once to give a value to global variable `text_element` and to make the initial call to `ShowAndGrowElement()`. `ShowAndGrowElement()` is called every 50 milliseconds and resets the [‘transform’](coords.md#TransformAttribute) and [‘style’](styling.md#StyleAttribute) attributes on the text element to new values each time it is called. At the end of `ShowAndGrowElement`, the function tells the ECMAScript engine to call itself again after 50 more milliseconds.
- The [‘g’](struct.md#GElement) element shifts the coordinate system so that the origin is shifted toward the lower-left of the viewing area. It also defines the fill color and font-size to use when drawing the text string.
- The [‘text’](text.md#TextElement) element contains the text string and is the element whose attributes get changed during the animation.

If scripts are modifying the same attributes or properties that are being animated by SVG's [animation elements](#AnimationElements), the scripts modify the base value for the animation. If a base value is modified while an animation element is animating the corresponding attribute or property, the animations are required to adjust dynamically to the new base value.

If a script is modifying a property on the override style sheet at the same time that an [animation element](intro.md#TermAnimationElement) is animating that property, the result is implementation-dependent; thus, it is recommended that this be avoided.

## <a id="DOMInterfaces"></a>19.4 DOM interfaces

Below are the DOM interfaces for the elements defined in this chapter. In addition, [ElementTimeControl](#InterfaceElementTimeControl) and [TimeEvent](#InterfaceTimeEvent), which are from SMIL Animation, are included here for easy reference.

### <a id="InterfaceElementTimeControl"></a>19.4.1 Interface ElementTimeControl

SMIL Animation supports several methods for controlling the behavior of animation: `beginElement()`, `beginElementAt()`, `endElement()` and `endElementAt()`. These methods are used to begin and end the active duration of an element. Authors can (but are not required to) declare the timing to respond to the DOM using the following syntax:

```
<animate begin="indefinite" end="indefinite" .../>
```

If a DOM method call is made to begin or end the element (using `beginElement()`, `beginElementAt()`, `endElement()` or `endElementAt()`), each method call creates a single instance time (in the appropriate instance times list). These times are then interpreted as part of the semantics of lists of times, as described in Evaluation of begin and end time lists.

- The instance time associated with a `beginElement()` or `endElement()` call is the current presentation time at the time of the DOM method call.
- The instance time associated with a `beginElementAt()` or `endElementAt()` call is the current presentation time at the time of the DOM method call, plus or minus the specified offset.
- Note that `beginElement()` is subject to the [‘restart’](#RestartAttribute) attribute in the same manner that event-based begin timing is. Refer also to SMIL Animation: Restarting animation ([[SMILANIM](refs.md#ref-SMILANIM)], section 3.3.7).

The expectation of the following interface is that an instance of the [ElementTimeControl](#InterfaceElementTimeControl) interface can be obtained by using binding-specific casting methods on an instance of an animation element. A DOM application can use the hasFeature method of the DOMImplementation interface to determine whether the [ElementTimeControl](#InterfaceElementTimeControl) interface is supported or not. The feature string for this interface is "TimeControl".

```
interface ElementTimeControl {
  void beginElement();
  void beginElementAt(in float offset);
  void endElement();
  void endElementAt(in float offset);
};
```

**Operations:**
:

**<a id="__smil__ElementTimeControl__beginElement"></a>void **beginElement**()**
:

Creates a begin instance time for the current time. The new instance time is added to the *begin instance times list*. The behavior of this method is equivalent to `beginElementAt(0)`.

**<a id="__smil__ElementTimeControl__beginElementAt"></a>void **beginElementAt**(in float *offset*)**
:

Creates a begin instance time for the current time plus the specified offset. The new instance time is added to the *begin instance times list*.

**Parameters**
:

1.
float *offset*
The offset from the current document time, in seconds, at which to begin the element.

**<a id="__smil__ElementTimeControl__endElement"></a>void **endElement**()**
:

Creates an end instance time for the current time. The new instance time is added to the *end instance times list*. The behavior of this method is equivalent to `endElementAt(0)`.

**<a id="__smil__ElementTimeControl__endElementAt"></a>void **endElementAt**(in float *offset*)**
:

Creates a end instance time for the current time plus the specified offset. The new instance time is added to the *end instance times list*.

**Parameters**
:

1.
float *offset*
offset from the current document time, in seconds, at which to end the element.

For the corresponding Java binding, see section 6.4 of SMIL Animation [[SMILANIM](refs.md#ref-SMILANIM)].

### <a id="InterfaceTimeEvent"></a>19.4.2 Interface TimeEvent

The [TimeEvent](#InterfaceTimeEvent) interface, defined in SMIL Animation: Supported interfaces, provides specific contextual information associated with Time events.

The different types of events that can occur are:

****beginEvent****
: This event is raised when the element local timeline begins to play. It will be raised each time the element begins the active duration (i.e. when it restarts, but not when it repeats). It may be raised both in the course of normal (i.e. scheduled or interactive) timeline play, as well as in the case that the element was begun with the [beginElement](#__smil__ElementTimeControl__beginElement) or [beginElementAt](#__smil__ElementTimeControl__beginElementAt) methods. Note that if an element is restarted while it is currently playing, the element will raise an end event and another begin event, as the element restarts.

- Bubbles: No
- Cancelable: No
- Context Info: None

****endEvent****
: This event is raised at the active end of the element. Note that this event is not raised at the simple end of each repeat. This event may be raised both in the course of normal (i.e. scheduled or interactive) timeline play, as well as in the case that the element was ended with the [endElement](#__smil__ElementTimeControl__endElement) or [endElementAt](#__smil__ElementTimeControl__endElementAt) methods. Note that if an element is restarted while it is currently playing, the element will raise an end event and another begin event, as the element restarts.

- Bubbles: No
- Cancelable: No
- Context Info: None

****repeatEvent****
: This event is raised when an element local timeline repeats. It will be raised each time the element repeats, after the first iteration. The event provides a numerical indication of which repeat iteration is beginning. The value is a 0-based integer, but the repeat event is not raised for the first iteration and so the observed values of the detail attribute will be >= 1.

- Bubbles: No
- Cancelable: No
- Context Info: detail (current iteration)

```
interface TimeEvent : Event {

  readonly attribute AbstractView view;
  readonly attribute long detail;

  void initTimeEvent(in DOMString typeArg, in AbstractView viewArg, in long detailArg);
};
```

**Attributes:**
:

**<a id="__smil__TimeEvent__view"></a>**view** (readonly AbstractView)**
:

The [view](#__smil__TimeEvent__view) attribute identifies the AbstractView [[DOM2VIEWS](refs.md#ref-DOM2VIEWS)] from which the event was generated.

**<a id="__smil__TimeEvent__detail"></a>**detail** (readonly long)**
:

Specifies some detail information about the Event, depending on the type of the event. For this event type, indicates the repeat number for the animation.

**Operations:**
:

**<a id="__smil__TimeEvent__initTimeEvent"></a>void **initTimeEvent**(in DOMString *typeArg*, in AbstractView *viewArg*, in long *detailArg*)**
:

The [initTimeEvent](#__smil__TimeEvent__initTimeEvent) method is used to initialize the value of a [TimeEvent](#InterfaceTimeEvent) created through the DocumentEvent interface. This method may only be called before the [TimeEvent](#InterfaceTimeEvent) has been dispatched via the dispatchEvent method, though it may be called multiple times during that phase if necessary. If called multiple times, the final invocation takes precedence.

**Parameters**
:

1.
DOMString *typeArg*
Specifies the event type.
2.
AbstractView *viewArg*
Specifies the Event's AbstractView.
3.
long *detailArg*
Specifies the Event's detail.

For the corresponding Java binding, see section 6.4 of SMIL Animation [[SMILANIM](refs.md#ref-SMILANIM)].

### <a id="InterfaceSVGAnimationElement"></a>19.4.3 Interface SVGAnimationElement

The [SVGAnimationElement](#InterfaceSVGAnimationElement) interface is the base interface for all of the animation element interfaces: [SVGAnimateElement](#InterfaceSVGAnimateElement), [SVGSetElement](#InterfaceSVGSetElement), [SVGAnimateColorElement](#InterfaceSVGAnimateColorElement), [SVGAnimateMotionElement](#InterfaceSVGAnimateMotionElement) and [SVGAnimateTransformElement](#InterfaceSVGAnimateTransformElement).

Unlike other SVG DOM interfaces, the SVG DOM does not specify convenience DOM properties corresponding to the various language attributes on SVG's animation elements. Specification of these convenience properties in a way that will be compatible with future versions of SMIL Animation is expected in a future version of SVG. The current method for accessing and modifying the attributes on the animation elements is to use the standard `getAttribute`, `setAttribute`, `getAttributeNS` and `setAttributeNS` defined in DOM Level 2 Core [[DOM2](refs.md#ref-DOM2)].

```
interface SVGAnimationElement : SVGElement,
                                SVGTests,
                                SVGExternalResourcesRequired,
                                ElementTimeControl {

  readonly attribute SVGElement targetElement;

  float getStartTime() raises(DOMException);
  float getCurrentTime();
  float getSimpleDuration() raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimationElement__targetElement"></a>**targetElement** (readonly [SVGElement](types.md#InterfaceSVGElement))**
:

The element which is being animated.

**Operations:**
:

**<a id="__svg__SVGAnimationElement__getStartTime"></a>float **getStartTime**()**
:

Returns the begin time, in seconds, for this animation element's current interval, if it exists, regardless of whether the interval has begun yet. If there is no current interval, then a DOMException with code INVALID_STATE_ERR is thrown.

**Returns**
:

The start time, in seconds, of this animation element's current interval.

**Exceptions**
:

**DOMException, code INVALID_STATE_ERR**
: The animation element does not have a current interval.
**<a id="__svg__SVGAnimationElement__getCurrentTime"></a>float **getCurrentTime**()**
:

Returns the current time in seconds relative to time zero for the given time container.

**Returns**
:

The current time in seconds relative to time zero for the given time container.

**<a id="__svg__SVGAnimationElement__getSimpleDuration"></a>float **getSimpleDuration**()**
:

Returns the number of seconds for the simple duration for this animation. If the simple duration is undefined (e.g., the end time is indefinite), then an exception is raised.

**Returns**
:

number of seconds for the simple duration for this animation.

**Exceptions**
:

**DOMException, code NOT_SUPPORTED_ERR**
: The simple duration is not determined on the given element.
### <a id="InterfaceSVGAnimateElement"></a>19.4.4 Interface SVGAnimateElement

The [SVGAnimateElement](#InterfaceSVGAnimateElement) interface corresponds to the [‘animate’](#AnimateElement) element.

Object-oriented access to the attributes of the [‘animate’](#AnimateElement) element via the SVG DOM is not available.

```
interface SVGAnimateElement : SVGAnimationElement,
                              SVGStylable {
};
```

### <a id="InterfaceSVGSetElement"></a>19.4.5 Interface SVGSetElement

The [SVGSetElement](#InterfaceSVGSetElement) interface corresponds to the [‘set’](#SetElement) element.

Object-oriented access to the attributes of the [‘set’](#SetElement) element via the SVG DOM is not available.

```
interface SVGSetElement : SVGAnimationElement {
};
```

### <a id="InterfaceSVGAnimateMotionElement"></a>19.4.6 Interface SVGAnimateMotionElement

The [SVGAnimateMotionElement](#InterfaceSVGAnimateMotionElement) interface corresponds to the [‘animateMotion’](#AnimateMotionElement) element.

Object-oriented access to the attributes of the [‘animateMotion’](#AnimateMotionElement) element via the SVG DOM is not available.

```
interface SVGAnimateMotionElement : SVGAnimationElement {
};
```

### <a id="InterfaceSVGMPathElement"></a>19.4.7 Interface SVGMPathElement

The [SVGMPathElement](#InterfaceSVGMPathElement) interface corresponds to the [‘mpath’](#MPathElement) element.

```
interface SVGMPathElement : SVGElement,
                            SVGURIReference,
                            SVGExternalResourcesRequired {
};
```

### <a id="InterfaceSVGAnimateColorElement"></a>19.4.8 Interface SVGAnimateColorElement

The [SVGAnimateColorElement](#InterfaceSVGAnimateColorElement) interface corresponds to the [‘animateColor’](#AnimateColorElement) element.

Object-oriented access to the attributes of the [‘animateColor’](#AnimateColorElement) element via the SVG DOM is not available.

```
interface SVGAnimateColorElement : SVGAnimationElement,
                                   SVGStylable {
};
```

### <a id="InterfaceSVGAnimateTransformElement"></a>19.4.9 Interface SVGAnimateTransformElement

The [SVGAnimateTransformElement](#InterfaceSVGAnimateTransformElement) interface corresponds to the [‘animateTransform’](#AnimateTransformElement) element.

Object-oriented access to the attributes of the [‘animateTransform’](#AnimateTransformElement) element via the SVG DOM is not available.

```
interface SVGAnimateTransformElement : SVGAnimationElement {
};
```

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](https://www.w3.org/TR/SVG11/script.html) ⋅ [Next](fonts.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
