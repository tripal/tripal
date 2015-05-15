<?php

require_once 'tripal_cv.vocabulary.inc';
require_once 'tripal_cv.vocabulary_term.inc';
require_once 'tripal_cv.organism.inc';
require_once 'tripal_cv.feature.inc';

/**
 * Implements hook_entity_info().
 */
function tripal_cv_entity_info() {
  $entities = array();

  // The vocabulary and vocabulary term entities.
  $entities['trp_vocabulary'] = array(
    'label' => t('Controlled Vocabulary'),
    'plural label' => t('Controlled Vocabularies'),
    'entity class' => 'TrpVocabulary',
    'controller class' => 'TrpVocabularyController',
    'entity keys' => array(
      'id' => 'internal_id'
    ),
  );
  $entities['trp_vocabulary_term'] = array(
    'label' => t('Controlled Vocabulary Term'),
    'plural label' => t('Controlled Vocabulary Terms'),
    'entity class' => 'TrpVocabularyTerm',
    'controller class' => 'TrpVocabularyTermController',
    'entity keys' => array(
      'id' => 'internal_id'
    ),
  );

  // We want to dynamically add entities for terms specifically set
  // by the site admin
  $values = array(
    'cv_id' => array(
      'name' => 'sequence'
    ),
    'name' => 'gene'
  );
  $cvterm = chado_generate_var('cvterm', $values);
  $entity_id = 'trp_' . $cvterm->dbxref_id->db_id->name . '_' . $cvterm->dbxref_id->accession;
  $label = preg_replace('/_/', ' ', ucwords($cvterm->name));
  $plural_label = $label . '(s)';
  $entity_class = 'TrpTerm' . $entity_id;
  $entities[$entity_id] = array(
    'label' => $label,
    'plural label' => $plural_label,
    'entity class' => $entity_class,
    'controller class' => 'TrpFeatureController',
    'entity keys' => array(
      'id' => 'internal_id'
    ),
  );

  // The organism entity.
  $entities['trp_organism'] = array(
    'label' => t('Organism'),
    'plural label' => t('Organism'),
    'entity class' => 'TrpOrganism',
    'controller class' => 'TrpOrganismController',
    'entity keys' => array(
      'id' => 'internal_id'
    ),
  );
  return $entities;
}