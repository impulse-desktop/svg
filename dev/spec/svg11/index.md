<!-- https://www.w3.org/TR/SVG11/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="pagetitle"></a>Scalable Vector Graphics (SVG) 1.1 (Second Edition)

## <a id="pagesubtitle"></a>W3C Recommendation *16 August 2011*

**This version:**
: http://www.w3.org/TR/2011/REC-SVG11-20110816/
**Latest version:**
: http://www.w3.org/TR/SVG11/
**Previous version:**
: http://www.w3.org/TR/2011/PR-SVG11-20110609/
**Public comments:**
: www-svg@w3.org (archive)
**Editors:**
: Erik Dahlström, Opera Software <ed@opera.com>
: Patrick Dengler, Microsoft Corporation <patd@microsoft.com>
: Anthony Grasso, Canon Inc. <anthony.grasso@cisra.canon.com.au>
: Chris Lilley, W3C <chris@w3.org>
: Cameron McCormack, Mozilla Corporation <cam@mcc.id.au>
: Doug Schepers, W3C <schepers@w3.org>
: Jonathan Watt, Mozilla Corporation <jwatt@jwatt.org>
: Jon Ferraiolo, ex Adobe Systems <jferrai@us.ibm.com> (Versions 1.0 and 1.1 First Edition; until 10 May 2006)
: 藤沢 淳 (FUJISAWA Jun), Canon Inc. <fujisawa.jun@canon.co.jp> (Version 1.1 First Edition)
: Dean Jackson, ex W3C <dean@w3.org> (Version 1.1 First Edition; until February 2007)
Please refer to the **errata** for this document, which may include some normative corrections.

This document is also available in these non-normative formats: a [single-page version](https://www.w3.org/TR/SVG11/single-page.html), a [zip archive of HTML](https://www.w3.org/TR/SVG11/REC-SVG11-20110816.zip) (without external dependencies), and a [PDF](https://www.w3.org/TR/SVG11/REC-SVG11-20110816.pdf). See also **translations**, noting that the English version of this specification is the only normative version.

Copyright © 2011 W3C^® (MIT, ERCIM, Keio), All Rights Reserved. W3C liability, trademark and document use rules apply.

## <a id="abstract"></a>Abstract

This specification defines the features and syntax for Scalable Vector Graphics (SVG) Version 1.1, a modularized language for describing two-dimensional vector and mixed vector/raster graphics in XML.

## <a id="status"></a>Status of this document

*This section describes the status of this document at the time of its publication. Other documents may supersede this document. A list of current W3C publications and the latest revision of this technical report can be found in the W3C technical reports index at http://www.w3.org/TR/.*

This document is the 16 August 2011 SVG 1.1 Second Edition Recommendation. The Second Edition incorporates a number of corrections that were published as errata against the First Edition, as well as numerous other changes that help make the specification more readable and unambiguous. The [Changes](changes.md) appendix lists all of the changes that were made since the first Proposed Recommendation publication of the Second Edition. For all changes made between the First Edition and the Second Edition, see:

- Changes between the SVG 1.1 First Edition Recommendation and the first Last Call Working Draft of SVG 1.1 Second Edition
- Changes between the first and second Last Call Working Drafts of SVG 1.1 Second Edition
- Changes between the second Last Call Working Draft and the Proposed Recommendation of SVG 1.1 Second Edition
- [Changes between the Proposed Recommendation and this document](changes.md)

Comments on this Recommendation are welcome. Corrections against the specification will be published as errata, and subsequently will be incorporated into future editions of SVG 1.1 or into SVG 2.0. Comments can be sent to www-svg@w3.org, the public email list for issues related to vector graphics on the Web. This list is archived and senders must agree to have their message publicly archived from their first posting. To subscribe send an email to www-svg-request@w3.org with the word `subscribe` in the subject line.

The W3C SVG Working Group has released an expanded test suite for SVG 1.1 along with an implementation report. This test suite will continue to be updated with new tests to improve interoperability even after Recommendation phase.

This document has been produced by the W3C SVG Working Group as part of the Graphics Activity within the W3C Interaction Domain. The goals of the W3C SVG Working Group are discussed in the W3C SVG Charter. The W3C SVG Working Group maintains a public Web page, http://www.w3.org/Graphics/SVG/, that contains further background information. The authors of this document are the SVG Working Group participants.

This document has been reviewed by W3C Members, by software developers, and by other W3C groups and interested parties, and is endorsed by the Director as a W3C Recommendation. It is a stable document and may be used as reference material or cited from another document. W3C's role in making the Recommendation is to draw attention to the specification and to promote its widespread deployment. This enhances the functionality and interoperability of the Web.

This document was produced by a group operating under the 5 February 2004 W3C Patent Policy. W3C maintains a public list of any patent disclosures made in connection with the deliverables of the group; that page also includes instructions for disclosing a patent. An individual who has actual knowledge of a patent which the individual believes contains Essential Claim(s) must disclose the information in accordance with section 6 of the W3C Patent Policy.

### <a id="AvailableLanguages"></a>Available languages

The English version of this specification is the only normative version. However, for translations in other languages see http://www.w3.org/Graphics/SVG/svg-updates/translations.html.

<a id="minitoc-section"></a>

## <a id="minitoc"></a>Table of Contents

## <a id="Acknowledgments"></a>Acknowledgments

The SVG Working Group would like to thank the following people for contributing to this specification by raising issues that resulted in errata that were folded in to this document: Tavmjong Bah, Brian Birtles, Tolga Capin, Alex Danilo, Thomas DeWeese, Alexey Feldgendler, Vincent Hardy, Ian Hickson, Olaf Hoffmann, Daniel Holbert, Oliver Hunt, Anne van Kesteren, Takeshi Kurosawa, Paul Libbrecht, Robert Longson, Helder Magalhães, Robert O’Callahan, Olli Pettay, Antoine Quint, Kalle Raita, Tim Rowley, Peter Sorotokin, Henry S. Thompson, Jasper van de Gronde, Mohamed Zergaoui, Boris Zbarsky.

In addition, the SVG Working Group would like to acknowledge the contributions of the editors and authors of SVG 1.0 and SVG 1.1 (First Edition), as much of the text in this document derives from these earlier versions of the SVG specification.

Finally, the SVG Working Group would like to acknowledge the great many people outside of the SVG Working Group who help with the process of developing the SVG specifications. These people are too numerous to list individually. They include but are not limited to the early implementers of the SVG 1.0 and 1.1 languages (including viewers, authoring tools, and server-side transcoders), developers of SVG content, people who have contributed on the www-svg@w3.org and svg-developers@yahoogroups.com email lists, other Working Groups at the W3C, and the W3C Team. SVG 1.1 is truly a cooperative effort between the SVG Working Group, the rest of the W3C, and the public and benefits greatly from the pioneering work of early implementers and content developers, feedback from the public, and help from the W3C team.

SVG 1.1 (Second Edition) – 16 August 2011Top ⋅ Contents ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
