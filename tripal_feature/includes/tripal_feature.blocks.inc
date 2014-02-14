<?php
/**
 * @file
 *  Describes the feature blocks to drupal
 */

/**
 * Implements hook_block().
 *
 * @ingroup tripal_feature
 */
function tripal_feature_block_info() {

  $blocks['references']['info'] = t('Tripal Feature References');
  $blocks['references']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['base']['info'] = t('Tripal Feature Details');
  $blocks['base']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['sequence']['info'] = t('Tripal Feature Sequence');
  $blocks['sequence']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['featureloc_sequences']['info'] = t('Tripal Feature Annotated Sequence');
  $blocks['featureloc_sequences']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['synonyms']['info'] = t('Tripal Feature Synonyms');
  $blocks['synonyms']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['properties']['info'] = t('Tripal Feature Properties');
  $blocks['properties']['cache'] = 'BLOCK_NO_CACHE';;

  $blocks['terms']['info'] = t('Tripal Annotated Terms');
  $blocks['terms']['cache'] = 'BLOCK_NO_CACHE';;

  $blocks['alignments']['info'] = t('Tripal Feature Alignments');
  $blocks['alignments']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['relationships']['info'] = t('Tripal Feature Relationships');
  $blocks['relationships']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['org_feature_counts']['info'] = t('Tripal Organism Feature Counts');
  $blocks['org_feature_counts']['cache'] = 'BLOCK_NO_CACHE';

  $blocks['org_feature_browser']['info'] = t('Tripal Organism Feature Browser');
  $blocks['org_feature_browser']['cache'] = 'BLOCK_NO_CACHE';

  return $blocks;
}

/**
 * Implements hook_block_view().
 *
 * @ingroup tripal_feature
 */
function tripal_feature_block_view($delta = '') {

  if (user_access('access chado_feature content') and arg(0) == 'node' and is_numeric(arg(1))) {
    $nid = arg(1);
    $node = node_load($nid);

    $block = array();
    switch ($delta) {
      case 'references':
        $block['subject'] = t('Cross References');
        $block['content'] = theme('tripal_feature_references', $node);
        break;
      case 'base':
        $block['subject'] = t('Feature Details');
        $block['content'] = theme('tripal_feature_base', $node);
        break;
      case 'synonyms':
        $block['subject'] = t('Synonyms');
        $block['content'] = theme('tripal_feature_synonyms', $node);
        break;
      case 'properties':
        $block['subject'] = t('Properties');
        $block['content'] = theme('tripal_feature_properties', $node);
        break;
      case 'terms':
        $block['subject'] = t('Annotated Terms');
        $block['content'] = theme('tripal_feature_terms', $node);
        break;
      case 'sequence':
        $block['subject'] = t('Sequence');
        $block['content'] = theme('tripal_feature_sequence', $node);
        break;
      case 'featureloc_sequences':
        $block['subject'] = t('Formatted Sequences');
        $block['content'] = theme('tripal_feature_featureloc_sequences', $node);
        break;
      case 'alignments':
        $block['subject'] = t('Alignments');
        $block['content'] = theme('tripal_feature_alignments', $node);
        break;
      case 'relationships':
        $block['subject'] = t('Relationships');
        $block['content'] = theme('tripal_feature_relationships', $node);
        break;
      case 'org_feature_counts':
        $block['subject'] = t('Feature Type Summary');
        $block['content'] = theme('tripal_organism_feature_counts', array('node' => $node));
        break;
      case 'org_feature_browser':
        $block['subject'] = t('Feature Browser');
        $block['content'] = theme('tripal_organism_feature_browser', array('node' => $node));
        break;
      case 'library_feature_browser':
        $block['subject'] = t('Library Feature Browser');
        $block['content'] = theme('tripal_library_feature_browser', $node);
        break;
      case 'analysis_feature_browser':
        $block['subject'] = t('Analysis Feature Browser');
        $block['content'] = theme('tripal_analysis_feature_browser', $node);
        break;
      default :
    }
    return $block;
  }
}
