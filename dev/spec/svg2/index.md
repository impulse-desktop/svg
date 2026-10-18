<!-- https://svgwg.org/svg2-draft/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="pagetitle"></a>Scalable Vector Graphics (SVG) 2

## <a id="pagesubtitle"></a>W3C Editor’s Draft *14 September 2025*

**This version:**
: https://svgwg.org/svg2-draft/
**Latest version:**
: https://www.w3.org/TR/SVG2/
**Previous version:**
: https://www.w3.org/TR/2018/CR-SVG2-20180807/
**Single page version:**
: [https://svgwg.org/svg2-draft/single-page.html](https://svgwg.org/svg2-draft/single-page.html)
**GitHub repository:**
: https://github.com/w3c/svgwg/
**Public comments:**
: www-svg@w3.org (archive)
**Editors:**
: Amelia Bellamy-Royds, Invited Expert <amelia.bellamy.royds@gmail.com>
: Tavmjong Bah, Invited Expert <tavmjong@free.fr>
: Chris Lilley, W3C <chris@w3.org>
: Dirk Schulze, Adobe Systems <dschulze@adobe.com>
: Eric Willigers, Google
**Former Editors:**
: Nikos Andronikos, Canon, Inc. <nikos.andronikos@cisra.canon.com.au>
: Rossen Atanassov, Microsoft Co. <ratan@microsoft.com>
: Brian Birtles, Mozilla Japan <bbirtles@mozilla.com>
: Bogdan Brinza, Microsoft Co. <bbrinza@microsoft.com>
: Cyril Concolato, Telecom ParisTech <cyril.concolato@telecom-paristech.fr>
: Erik Dahlström, Invited Expert <erik@dahlström.net>
: Cameron McCormack, Mozilla Corporation <cam@mcc.id.au>
: David Storey, Microsoft Co. <dstorey@microsoft.com>
: Doug Schepers, W3C <schepers@w3.org>
: Richard Schwerdtfeger, IBM <schwer@us.ibm.com>
: Satoru Takagi, KDDI Corporation <sa-takagi@kddi.com>
: Jonathan Watt, Mozilla Corporation <jwatt@jwatt.org>
Copyright © 2025 W3C^® (MIT, ERCIM, Keio, Beihang). W3C liability, trademark and document use rules apply.

## <a id="abstract"></a>Abstract

This specification defines the features and syntax for Scalable Vector Graphics (SVG) Version 2. SVG is a language based on XML for describing two-dimensional vector and mixed vector/raster graphics. SVG content is stylable, scalable to different display resolutions, and can be viewed stand-alone, mixed with HTML content, or embedded using XML namespaces within other XML languages. SVG also supports dynamic changes; script can be used to create interactive documents, and animations can be performed using declarative animation features or by using script.

## <a id="status"></a>Status of This Document

*This section describes the status of this document at the time of its publication. Other documents may supersede this document. A list of current W3C publications and the latest revision of this technical report can be found in the W3C technical reports index at https://www.w3.org/TR/*.

This document is the 14 September 2025 **Editor’s Draft** of SVG 2. This version of SVG builds upon [SVG 1.1 Second Edition](../svg11/index.md) by improving the usability of the language and by adding new features commonly requested by authors. The [Changes](changes.md) appendix lists all of the changes that have been made since SVG 1.1 Second Edition.

Comments on this Editor’s Draft are welcome. Comments can be sent to www-svg@w3.org, the public email list for issues related to vector graphics on the Web. This list is archived and senders must agree to have their message publicly archived from their first posting. To subscribe send an email to www-svg-request@w3.org with the word `subscribe` in the subject line.

The specification includes a number of annotations that the Working Group is using to record links to meeting minutes and resolutions where specific decisions about SVG features have been made. Different coloring is also used to mark the maturity of different sections of the specification:

- a red background indicates a section that is either unchanged since SVG 1.1 (and which therefore still requires review and possible rewriting for SVG 2), or a section that is new but still requires substantial work
- a yellow background indicates a section from SVG 1.1 that has been reviewed and rewritten if necessary, or a new section that is complete and ready for the rest of the Working Group to review
- a white background indicates a section, either from SVG 1.1 or new for SVG 2, that has been reviewed by the Working Group and which is ready for wider review

This document has been produced by the W3C SVG Working Group as part of the Graphics Activity within the W3C Interaction Domain. The goals of the W3C SVG Working Group are discussed in the W3C SVG Charter. The W3C SVG Working Group maintains a public Web page, https://www.w3.org/Graphics/SVG/, that contains further background information. The authors of this document are the SVG Working Group participants.

This document was produced by a group operating under the 5 February 2004 W3C Patent Policy. W3C maintains a public list of any patent disclosures made in connection with the deliverables of the group; that page also includes instructions for disclosing a patent. An individual who has actual knowledge of a patent which the individual believes contains Essential Claim(s) must disclose the information in accordance with section 6 of the W3C Patent Policy.

Publication as a Working Draft does not imply endorsement by the W3C Membership. This is a draft document and may be updated, replaced or obsoleted by other documents at any time. It is inappropriate to cite this document as other than work in progress.

A list of current W3C Recommendations and other technical documents can be found at https://www.w3.org/TR/. W3C publications may be updated, replaced, or obsoleted by other documents at any time.

This document is governed by the <a id="w3c_process_revision"></a>1 September 2015 W3C Process Document.

All features in this specification depend upon implementation in browsers or authoring tools. If a feature is not certain to be implemented, we define that feature as "at risk". At-risk features will be removed from the current specification, and may be included in future versions of the specification. If an at-risk feature is particularly important to authors of SVG, those authors are encouraged to give feedback to implementers regarding its priority. The following features are at risk, and may be dropped during the CR period:

- More than one ‘[title](struct.md#TitleElement)’ or ‘[desc](struct.md#DescElement)’ to provide localisation
- [Nested links](linking.md#Links)
- [vector-effect](coords.md#VectorEffectProperty) options other than non-scaling-stroke
- [stroke-linejoin](painting.md#StrokeLinejoinProperty) options miter-clip and arcs
- the [shape-inside](text.md#TextShapeInside) and [shape-subtract](text.md#TextShapeSubtract) properties

## <a id="Acknowledgments"></a>Acknowledgments

The SVG Working Group would like to thank the following people for contributing to this specification with patches or by participating in discussions that resulted in changes to the document: David Dailey, Eric Eastwood, Jarek Foksa, Daniel Holbert, Paul LeBeau, Robert Longson, Henri Manson, Ms2ger, Kari Pihkala, Philip Rogers, David Zbarsky.

In addition, the SVG Working Group would like to acknowledge the contributions of the editors and authors of the previous versions of SVG – as much of the text in this document derives from these earlier specifications – including:

- Patrick Dengler, Microsoft Corporation (Version 1.1 Second Edition)
- Jon Ferraiolo, ex Adobe Systems (Versions 1.0 and 1.1 First Edition; until 10 May 2006)
- Anthony Grasso, ex Canon Inc. (Version 1.1 Second Edition)
- Dean Jackson, ex W3C (Version 1.1 First Edition; until February 2007)
- 藤沢 淳 (FUJISAWA Jun), Canon Inc. (Version 1.1 First Edition)

Finally, the SVG Working Group would like to acknowledge the great many people outside of the SVG Working Group who help with the process of developing the SVG specifications. These people are too numerous to list individually. They include but are not limited to the early implementers of the SVG 1.0 and 1.1 languages (including viewers, authoring tools, and server-side transcoders), developers of SVG content, people who have contributed on the www-svg@w3.org and svg-developers@yahoogroups.com email lists, other Working Groups at the W3C, and the W3C Team. SVG 1.1 is truly a cooperative effort between the SVG Working Group, the rest of the W3C, and the public and benefits greatly from the pioneering work of early implementers and content developers, feedback from the public, and help from the W3C team.
