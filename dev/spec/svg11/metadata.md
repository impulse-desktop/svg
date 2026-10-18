<!-- https://www.w3.org/TR/SVG11/metadata.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](fonts.md) ⋅ [Next](backward.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 21 Metadata

## <a id="Introduction"></a>21.1 Introduction

Metadata is structured data about data.

In the computing industry, there are ongoing standardization efforts towards metadata with the goal of promoting industry interoperability and efficiency. Content creators should track these developments and include appropriate metadata in their SVG content which conforms to these various metadata standards as they emerge.

The W3C has a Semantic Web Activity which has been established to serve a leadership role, in both the design of enabling specifications and the open, collaborative development of technologies that support the automation, integration and reuse of data across various applications. The Semantic Web Activity builds upon the earlier W3C Metadata Activity, including the definition of Resource Description Framework (RDF). The RDF Primer is the first in a set of six documents that define the Resource Description Framework [[RDF-PRIMER](refs.md#ref-RDF-PRIMER)].

Another activity relevant to most applications of metadata is the Dublin Core [[DCORE](refs.md#ref-DCORE)], which is a set of generally applicable core metadata properties (e.g., Title, Creator/Author, Subject, Description, etc.).

Individual industries or individual content creators are free to define their own metadata schema but are encouraged to follow existing metadata standards and use standard metadata schema wherever possible to promote interchange and interoperability. If a particular standard metadata schema does not meet your needs, then it is usually better to define an additional metadata schema in an existing framework such as RDF and to use custom metadata schema in combination with standard metadata schema, rather than totally ignore the standard schema.

## <a id="MetadataElement"></a>21.2 The ‘metadata’ element

Metadata which is included with SVG content should be specified within [‘metadata’](#MetadataElement) elements. The contents of the [‘metadata’](#MetadataElement) should be elements from other XML namespaces, with these elements from these namespaces expressed in a manner conforming with the Namespaces in XML Recommendation [[XML-NS](refs.md#ref-XML-NS)].

Authors should provide a [‘metadata’](#MetadataElement) child element to the [outermost svg element](intro.md#TermOutermostSVGElement) within a stand-alone SVG document. The [‘metadata’](#MetadataElement) child element to an [‘svg’](struct.md#SVGElement) element serves the purposes of identifying document-level metadata.

The DTD definitions of many of SVG's elements (particularly, container and text elements) place no restriction on the placement or number of the [‘metadata’](#MetadataElement) sub-elements. This flexibility is only present so that there will be a consistent content model for container elements, because some container elements in SVG allow for mixed content, and because the mixed content rules for XML ([[XML10](refs.md#ref-XML10)], section 3.2.2) do not permit the desired restrictions. Representations of future versions of the SVG language might use more expressive representations than DTDs which allow for more restrictive mixed content rules. It is strongly recommended that at most one [‘metadata’](#MetadataElement) element appear as a child of any particular element, and that this element appear before any other child elements (except possibly [‘desc’](struct.md#DescElement) or [‘title’](struct.md#TitleElement) elements) or character data content. If metadata-processing user agents need to choose among multiple [‘metadata’](#MetadataElement) elements for processing it should choose the first one.

‘metadata’

**Categories:**
: [Descriptive element](intro.md#TermDescriptiveElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)

**DOM Interfaces:**
:

- [SVGMetadataElement](#InterfaceSVGMetadataElement)

## <a id="Example"></a>21.3 An example

Here is an example of how metadata can be included in an SVG document. The example uses the Dublin Core version 1.1 schema. (Other XML-compatible metadata languages, including ones not based on RDF, can be used also.)

```
<?xml version="1.0" standalone="yes"?>
<svg width="4in" height="3in" version="1.1"
    xmlns = 'http://www.w3.org/2000/svg'>
    <desc xmlns:myfoo="http://example.org/myfoo">
      <myfoo:title>This is a financial report</myfoo:title>
      <myfoo:descr>The global description uses markup from the
        <myfoo:emph>myfoo</myfoo:emph> namespace.</myfoo:descr>
      <myfoo:scene><myfoo:what>widget $growth</myfoo:what>
      <myfoo:contains>$three $graph-bar</myfoo:contains>
        <myfoo:when>1998 $through 2000</myfoo:when> </myfoo:scene>
   </desc>
    <metadata>
      <rdf:RDF
           xmlns:rdf = "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
           xmlns:rdfs = "http://www.w3.org/2000/01/rdf-schema#"
           xmlns:dc = "http://purl.org/dc/elements/1.1/" >
        <rdf:Description about="http://example.org/myfoo"
             dc:title="MyFoo Financial Report"
             dc:description="$three $bar $thousands $dollars $from 1998 $through 2000"
             dc:publisher="Example Organization"
             dc:date="2000-04-11"
             dc:format="image/svg+xml"
             dc:language="en" >
          <dc:creator>
            <rdf:Bag>
              <rdf:li>Irving Bird</rdf:li>
              <rdf:li>Mary Lambert</rdf:li>
            </rdf:Bag>
          </dc:creator>
        </rdf:Description>
      </rdf:RDF>
    </metadata>
</svg>
```

## <a id="DOMInterfaces"></a>21.4 DOM interfaces

### <a id="InterfaceSVGMetadataElement"></a>21.4.1 Interface SVGMetadataElement

The [SVGMetadataElement](#InterfaceSVGMetadataElement) interface corresponds to the [‘metadata’](#MetadataElement) element.

```
interface SVGMetadataElement : SVGElement {
};
```

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](fonts.md) ⋅ [Next](backward.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
