<?php

/**
 * @file
 * Provides an application programming interface (API) for working with
 * TripalEntity content types (bundles) and their entities.
 *
 */

/**
 * @defgroup tripal_entities_api Tripal Entities
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with
 * TripalEntity content types (bundles) and their entities.
 *
 * Bundles (Content Types): Bundles are types of content in a Drupal site.
 * By default, Drupal provides the Basic Page and Article content types,
 * and Drupal allows a site developer to create new content types on-the-fly
 * using the administrative interface--no programming required.  Tripal also
 * provides several Content Type by default. During installation of Tripal the
 * Organism, Gene, Project, Analysis and other content types are created
 * automatically.  The site developer can then create new content types for
 * different biological data--again, without any programming required.
 *
 * In order to to assist with data exchange and use of common data formats,
 * Tripal Bundles are defined using a controlled vocabulary term (cvterm).
 * For example, a "Gene" Bundle is defined using the Sequence Ontology term for
 * gene whose term accession is: SO:0000704. This mapping allows Tripal to
 * compare content across Tripal sites, and expose data to computational tools
 * that understand these vocabularies. By default, Tripal uses Chado as its
 * primary data storage back-end.
 *
 * Entity: An entity is a discrete data record.  Entities are most commonly
 * seen as "pages" on a Drupal web site and are instances of a Bundle
 * (i.e content type). When data is published on a Tripal site such as
 * organisms, genes, germplasm, maps, etc., each record is represented by a
 * single entity with an entity ID as its only attribute. All other
 * information that the entity provides is made available via Fields.
 *
 * For more information please see:
 * http://tripal.info/tutorials/v3.x/developers-handbook/structure
 * @}
 *
 */

/**
 * Get Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalEntityType $bundle
 *   The Entity object for the Tripal Entity Type the title format is for.
 */
function tripal_get_title_format($bundle) {

  // Get the existing title format if it exists.
  $title_format = $bundle->getTitleFormat();

  // If there isn't yet a title format for this bundle/type then we should
  // determine the default.
  if (!$title_format) {
    $title_format = $bundle->getDefaultTitleFormat();
    $bundle->setTitleFormat($title_format);
    $bundle->save();
  }

  return $title_format;
}

/**
 * Determine the default title format to use for an entity.
 *
 * @param TripalBundle $bundle
 *   The Entity object for the Tripal Bundle that the title format is for.
 *
 * @return string
 *   A default title format.
 *
 * @ingroup tripal_entities_api
 */
function tripal_get_default_title_format($bundle) {
  return $bundle->getDefaultTitleFormat();
}


/**
 * Returns an array of tokens based on Tripal Entity Fields.
 *
 * @param TripalBundle $bundle
 *    The bundle entity for which you want tokens.
 *
 * @return
 *    An array of tokens where the key is the machine_name of the token.
 */
function tripal_get_entity_tokens($bundle, $options = []) {
  return $bundle->getTokens($options);
}

/**
 * Replace all Tripal Tokens in a given string.
 *
 * NOTE: If there is no value for a token then the token is removed.
 *
 * @param string $string
 *   The string containing tokens.
 * @param TripalEntity $entity
 *   The entity with field values used to find values of tokens.
 * @param TripalBundle $bundle_entity
 *   The bundle entity containing special values sometimes needed for token
 *   replacement.
 *
 * @return
 *   The string will all tokens replaced with values.
 *
 * @ingroup tripal_entities_api
 */
function tripal_replace_entity_tokens($string, &$entity, $bundle_entity = NULL) {
  if ($bundle_entity) {
    return $entity->replaceTokens($string,
      ['tripal_entity_type' => $bundle_entity]);
  }
  else {
    return $entity->replaceTokens($string);
  }
}

/**
 * Formats the tokens for display.
 *
 * @param array $tokens
 *   A list of tokens generated via tripal_get_entity_tokens().
 *
 * @return
 *   A render array defining the available tokens.
 */
function theme_token_list($tokens) {

  $header = ['Token', 'Name', 'Description'];
  $rows = [];
  foreach ($tokens as $details) {
    $rows[] = [
      $details['token'],
      $details['label'],
      $details['description'],
    ];
  }

  return [
    '#type' => 'table',
    '#header' => $header,
    '#rows' => $rows
  ];
}
