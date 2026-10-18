<!-- https://www.w3.org/TR/SVG11/backward.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](metadata.md) ⋅ [Next](extend.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# <a id="BackwardsCompatibility"></a>22 Backwards Compatibility

A user agent (UA) might not have the ability to process and view SVG content. The following list outlines two of the backwards compatibility scenarios associated with SVG content:

-
For XML grammars with the ability to embed SVG content, it is assumed that some sort of alternate representation capability such as the [‘switch’](struct.md#SwitchElement) element and some sort of feature-availability test facility (such as what is described in the SMIL 3.0 specification [[SMIL](refs.md#ref-SMIL)]) will be available.
This [‘switch’](struct.md#SwitchElement) element and feature-availability test facility (or their equivalents) are the recommended way for XML authors to provide an alternate representation to SVG content, such as an image or a text string. The following example shows how to embed an SVG drawing within a SMIL 1.0 document such that an alternate image will display in the event the user agent doesn't support SVG. Note that the MIME type in the ‘type’ attribute is an important means for the user agent to decide if it can decode the referenced media.
In this example, the SVG content is included via a URL reference. With some parent XML grammars it will also be possible to include an SVG document fragment inline within the same file as its parent grammar.
```
<?xml version="1.0" standalone="yes"?>
<smil>
  <body>
    <!-- With SMIL 1.0, the first child element of 'switch'
         which the SMIL 1.0 user agent is able to process 
         and which tests true will get processed and all other 
         child elements will have no visual effect. In this case, 
         if the SMIL 1.0 user agent can process "image/svg+xml", 
         then the SVG will appear; otherwise, the alternate image 
         (the second child element) will appear. -->
    <switch>
      <!-- Render the SVG if possible. -->
      <ref type="image/svg+xml" src="drawing.svg" />
      <!-- Else, render the alternate image. -->
      <img src="alternate_image.jpg" />
    </switch>
  </body>
</smil>
```

-
For HTML 4, SVG drawings can be embedded using the ‘object’ element. An alternate representation such as an image can be included as the content of the ‘object’ element. In this case, the SVG content usually will be included via a URL reference. The following example shows how to use the ‘object’ element to include an SVG drawing via a URL reference with an image serving as the alternate representation in the absence of an SVG user agent:
```
<html>
  <body>
    <object type="image/svg+xml" data="drawing.svg">
      <!-- The contents of the 'object' element (i.e., an alternate
         image) are drawn in the event the user agent cannot process 
         the SVG drawing. -->
      <img src="alternate_image.jpg" alt="alternate description">
    </object>
  </body>
</html>
```

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](metadata.md) ⋅ [Next](extend.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
