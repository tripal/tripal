<?php

/**
 *
 */
class TripalTermViewsController extends EntityDefaultViewsController {

  /**
   * Edit or add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    $data['tripal_term']['table']['group'] = 'Tripal Term';

    $data['tripal_term']['accession']['title'] = 'Term Accession';
    $data['tripal_term']['accession']['help'] = t('The unique term ID (accession) of the vocabulary term to which a Tripal content type is asscoiated.');

    $data['tripal_term']['name']['title'] = 'Term Name';
    $data['tripal_term']['name']['help'] = t('The term that a content type is associated with.');
    $data['tripal_term']['name']['filter']['handler'] = 'tripal_views_handler_filter_string_selectbox';

    $data['tripal_term']['vocab_id']['title'] = t('Vocabulary ID');
    $data['tripal_term']['vocab_id']['help'] = t('The internal numeric ID for the vocabulary to which the content type term belongs.');
    $data['tripal_term']['vocab_id']['relationship'] = [
      'base' => 'tripal_vocab',
      'base field' => 'id',
      'handler' => 'views_handler_relationship',
      'label' => t('Tripal Content Vocabulary'),
      'title' => t('Tripal Content Vocabulary'),
      'help' => t('Associates the vocabulary ID of the Tripal content type with information about the vocabulary.'),
    ];


    $data['tripal_term']['table']['join'] = [];

    // Join the term to it's entity type.
    $data['tripal_term']['table']['join']['tripal_entity'] = [
      'handler' => 'views_join',
      'left_field' => 'term_id',
      'field' => 'id',
    ];

    // It is not intended that the following fields will every be used by the
    // end-user.
    unset($data['tripal_term']['id']);
    unset($data['tripal_term']['changed']);
    unset($data['tripal_term']['created']);

    // This should not be a base table. It's not useful to create a
    // view of the tripal_term table.
    unset($data['tripal_term']['table']['base']);

    return $data;
  }

}
