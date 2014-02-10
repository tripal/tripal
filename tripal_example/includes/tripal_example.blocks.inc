<?php

/**
 * @ingroup tripal_example
 */
function tripal_example_block_info() {

  $blocks['libreferences']['info'] = t('Tripal Example Cross References');
  $blocks['libreferences']['cache'] = DRUPAL_NO_CACHE;

  $blocks['libbase']['info'] = t('Tripal Example Details');
  $blocks['libbase']['cache'] = DRUPAL_NO_CACHE;

  $blocks['libterms']['info'] = t('Tripal Example Terms');
  $blocks['libterms']['cache'] = DRUPAL_NO_CACHE;

  $blocks['libsynonyms']['info'] = t('Tripal Example Synonyms');
  $blocks['libsynonyms']['cache'] = DRUPAL_NO_CACHE;

  $blocks['libproperties']['info'] = t('Tripal Example Properties');
  $blocks['libproperties']['cache'] = DRUPAL_NO_CACHE;

  $blocks['featurelibs']['info'] = t('Tripal Feature Examples');
  $blocks['featurelibs']['cache'] = DRUPAL_NO_CACHE;

  $blocks['orglibs']['info'] = t('Tripal Organism Examples');
  $blocks['orglibs']['cache'] = DRUPAL_NO_CACHE;

  return $blocks;
}


/**
 * @ingroup tripal_example
 */
function tripal_example_block_view($delta = '') {

  if (user_access('access chado_example content') and arg(0) == 'node' and is_numeric(arg(1))) {
    $nid = arg(1);
    $node = node_load($nid);

    $block = array();
    switch ($delta) {
      case 'libreferences':
        $block['subject'] = t('Cross References');
        $block['content'] = theme('tripal_example_references', $node);
        break;
      case 'libbase':
        $block['subject'] = t('Example Details');
        $block['content'] = theme('tripal_example_base', $node);
        break;
      case 'libsynonyms':
        $block['subject'] = t('Synonyms');
        $block['content'] = theme('tripal_example_synonyms', $node);
        break;
      case 'libproperties':
        $block['subject'] = t('Properties');
        $block['content'] = theme('tripal_example_properties', $node);
        break;
      case 'libterms':
        $block['subject'] = t('Example Terms');
        $block['content'] = theme('tripal_example_terms', $node);
        break;
      case 'featurelibs':
        $block['subject'] = t('Examples');
        $block['content'] = theme('tripal_feature_examples', $node);
        break;
      case 'orglibs':
        $block['subject'] = t('Examples');
        $block['content'] = theme('tripal_organism_examples', $node);
        break;
      default :
    }
    return $block;
  }
}