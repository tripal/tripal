<?php

/**
 *
 */
class TripalVocabViewsController extends EntityDefaultViewsController {

  /**
   * Edit or add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    $data['tripal_vocab']['table']['group'] = 'Tripal Term Vocabulary';

    $data['tripal_vocab']['vocabulary']['title'] = 'Vocabulary';
    $data['tripal_vocab']['vocabulary']['help'] = t('The short name of the vocabulary.');


    // Join the term to it's entity type.
    $data['tripal_vocab']['table']['join']['tripal_term'] = [
      'handler' => 'views_join',
      'left_field' => 'vocab_id',
      'field' => 'id',
    ];

    // It is not intended that the following fields will every be used by the
    // end-user.
    unset($data['tripal_vocab']['id']);
    unset($data['tripal_vocab']['changed']);
    unset($data['tripal_vocab']['created']);

    // This should not be a base table. It's not useful to create a
    // view of the tripal_term table.
    unset($data['tripal_vocab']['table']['base']);

    return $data;
  }

}
