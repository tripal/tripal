<?php

/**
 * @file
 * OWL redesign using Classes (Objects)
 */
class OWLStanza {

  // The XMLReader object.
  private $owl = NULL;

  // A boolean (TRUE or FALSE) indicating if children should be parsed.
  private $parse_children = TRUE;

  // The XML element tag name (e.g. ‘owl:ObjectProperty’,
  // ‘owl:AnnotationProperty’, 'owl:Ontology').
  private $tag_name = '';

  // A key/value associative array of all of the attributes.
  private $attributes = [];

  // An array of OWLStanza objects.
  private $children = [];

  // The text value of a XML element.
  private $value;

  // Indicates if there is no more to read from the OWL XML file.
  private $is_finished = FALSE;


  /**
   * Implements the constructor.
   *
   * @param $parse_children
   * A boolean (TRUE or FALSE) indicating if children should be parsed.
   * If FALSE only attributes will be parse.
   */
  public function __construct($owl, $parse_children = TRUE) {
    $this->owl = $owl;
    $this->parse_children = $parse_children;
    $this->parse();
  }

  /**
   * A wrapper for XMLReader::read() read function.
   *
   * This function calls read() on the $owl object and checks to see
   * if we are at the end of the XML file. If so, it sets the $is_finished
   * member variable to TRUE.
   *
   * @return
   *   The value of XMLReader::read().
   */
  private function owl_read() {
    $retval = $this->owl->read();

    if ($this->owl->nodeType == XMLReader::END_ELEMENT and $this->owl->name == 'rdf:RDF') {
      $this->is_finished = TRUE;
    }
    return $retval;
  }

  /**
   *
   * @param private
   * This function is to parse each of the OWL Stanzas of the ro.owl file.
   */

  private function parse() {

    // Make sure we are at the beginning of an element.
    while ($this->owl->nodeType != XMLReader::ELEMENT) {
      // In the event we've hit the end of the file, then return.
      if ($this->is_finished) {
        return;
      }
      $this->owl_read();
    }

    $this->tag_name = $this->owl->name;

    $has_value = $this->owl->hasValue;
    $is_empty = $this->owl->isEmptyElement;

    // Get the attributes.
    $num_attrs = $this->owl->attributeCount;
    if ($num_attrs > 0) {
      $this->owl->moveToFirstAttribute();
      for ($i = 0; $i < $num_attrs; $i++) {
        $this->attributes[$this->owl->name] = $this->owl->value;
        $this->owl->moveToNextAttribute();
      }
    }

    // If this element is empty then just return.
    if ($is_empty) {
      return;
    }

    // Determine if the element has a value. If so, then set the value for the class.
    if ($this->owl->hasValue) {
      $this->owl_read();
      $this->value = $this->owl->value;
    }

    // Get the children that should be parsed within the Stanza.
    if ($this->parse_children == TRUE) {
      while ($this->owl_read()) {
        if ($this->owl->nodeType == XMLReader::END_ELEMENT and $this->owl->name == $this->tag_name) {
          return;
        }
        else {
          if ($this->owl->nodeType == XMLReader::ELEMENT) {
            $child = new OWLStanza($this->owl);
            $this->children[] = $child;
          }
        }
      }
    }
  }


  /**
   * Get the Value
   *
   * @ return The text value of a XML element.
   */

  public function getValue() {
    return $this->value;
  }

  /**
   * Gets the children array.
   *
   * @return An array of OWLStanza objects containing the children elements.
   */
  public function getChildren() {
    return $this->children;
  }

  /**
   * Sets the children array.
   *
   * @param $children
   * An array of OWLStanza objects containing the children elements.
   *
   * @return FALSE if the array was not set, TRUE otherwise.
   */
  public function setChildren($children) {
    // Makes sure the incoming argument is an array.
    if (!is_array($children)) {
      return FALSE;
    }
    // Make sure that all of the array elements are OWLStanza objects.
    foreach ($children as $child) {
      if (get_class($child) != 'OWLStanza') {
        return FALSE;
      }
    }
    // All is good, set the children.
    $this->children = $children;
  }

  /**
   * Gets the child($tag_name) array.
   *
   * @return
   *  An OWLStanza object whos tag name matches the tag name provided.
   *
   *
   */
  public function getChild($tag_name) {
    foreach ($this->children as $child) {
      if ($child->getTagName() == $tag_name) {
        return $child;
      }
    }
    return NULL;
  }

  /**
   * Gets the tag name.
   *
   * @return An XML element tag name (e.g. ‘owl:ObjectProperty’,
   *         'owl:AnnotationProperty’, 'owl:Ontology').
   */
  public function getTagName() {
    return $this->tag_name;
  }

  /**
   * Sets the tag name.
   *
   * @param $tag_name The
   *          XML element tag name
   *
   * @return value element tag name.
   */
  public function setTagName($tag_name) {
    $this->tag_name = $tag_name;
    return;
  }

  /**
   * Gets the attributes array.
   *
   * @return An array containing a key/value associative array
   *         of all of the attributes between the XML elements tag name.
   */
  public function getAttributes() {
    return $this->attributes;
  }

  /**
   * Sets the attributes array.
   *
   * @param $attributes An
   *          array containing a key/value associative array of all of the
   *   attributes.
   *
   * @return FALSE if the array was not set, TRUE otherwise.
   */
  public function setAttributes($attributes) {
    // Makes sure the incoming argument is an array.
    if (!is_array($attributes)) {
      return FALSE;
    }
    // Make sure that all of the array are key/values.
    foreach ($attributes as $attribute_name) {
      if (get_class($attribute_name) != 'OWLStanza') {
        return FALSE;
      }
    }
    // All is good, set the attributes.
    $this->attributes = $attributes;
  }

  /**
   *
   * @param
   *          $attribute_name
   *
   * @return An|NULL
   */
  public function getAttribute($attribute_name) {
    foreach ($this->attributes as $aname => $value) {
      if ($aname == $attribute_name) {
        return $value;
      }
    }
    return NULL;

  }

  /**
   * Checks if the OWL XML file has been completely parsed.
   *
   * @return
   *   TRUE if parsing is completed, FALSE otherwise.
   */
  public function isFinished() {
    return $this->is_finished;
  }

  /**
   * Reconstructs the XML for the stanza.
   *
   * @return
   *   A string containing XML for this stanza.
   */
  public function getXML() {

    // Start the element with the tag name.
    $xml = '<' . $this->tag_name;

    // Iterate through the attributes and add them to our XML string.
    if (count($this->attributes) > 0) {
      foreach ($this->attributes as $aname => $value) {
        $xml .= " " . $aname . '="' . $value . '"';
      }
    }

    // If this stanza has a value this implies there are no children,
    // so close the element start, add the value and add the closing element
    // tag.
    if ($this->value) {
      $xml .= '>' . $this->value . '</' . $this->tag_name . ">\n";
      return $xml;
    }

    // If we're here, we do not have a value. This is therefore an empty
    // element, or it has children.  If we have no children then we
    // have an empty element and we can close it out and return.
    if (count($this->children) == 0) {
      $xml .= " />\n";
      return $xml;
    }

    // Add in the children's XML recursively.
    $childs = '';
    foreach ($this->children as $child) {
      // We want to add two character indentation to all lines returned by
      // the child XML.
      $childs .= preg_replace("/^/", "  ", $child->getXML());
    }
    $xml .= ">\n";
    $xml .= $childs;
    $xml .= "</" . $this->tag_name . ">\n";
    return $xml;
  }
}








