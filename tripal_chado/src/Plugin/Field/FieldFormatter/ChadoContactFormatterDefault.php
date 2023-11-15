<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal contact formatter.
 *
 * @FieldFormatter(
 *   id = "chado_contact_formatter_default",
 *   label = @Translation("Chado contact formatter"),
 *   description = @Translation("A chado contact formatter"),
 *   field_types = {
 *     "chado_contact_default"
 *   }
 * )
 */
class ChadoContactFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '[name]';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $list = [];
    $token_string = $items->getSetting('token_string');
    foreach($items as $delta => $item) {
      $values = [
        'name' => $item->get('contact_name')->getString(),
        'type' => $item->get('contact_type')->getString(),
        'description' => $item->get('contact_description')->getString(),
      ];

      // Change the non-user-friendly 'null' contact, which is specified by chado.
      if ($values['name'] == 'null') {
        $values['name'] = 'Unknown';
      }

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }
      $list[$delta] = $displayed_string;
    }

    // Only return markup if the field is not empty.
    if (!empty($list)) {

      // If only one element has been found, don't make into a list.
      if (count($list) == 1) {
        $elements[0] = [
          "#markup" => $list[0]
        ];
      }

      // If more than one value has been found, display all values in an
      // unordered list.
// @todo: add a pager
      elseif (count($list) > 1) {
        $elements[0] = [
          '#theme' => 'item_list',
          '#list_type' => 'ul',
          '#items' => $list,
          '#wrapper_attributes' => ['class' => 'container'],
        ];
      }
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['token_string'] = [
      '#title' => $this->t('Token string for field display'),
      '#description' => $this->t('You may specify elements in this text box to customize how'
                     . ' contacts are displayed. The available tokens are [name] for the'
                     . ' contact name, [type] for the type of contact (person, institution, etc.)'
                     . ' and [description] for the description for the contact.'
                     . ' For example, "[name] ([type]) [[description]]"'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('token_string'),
      '#element_validate' => [[static::class, 'settingsFormValidateTokenString']],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Set display format');
    return $summary;
  }

  /**
   * Form element validation handler for token string
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) form.
   */
  public static function settingsFormValidateTokenString(array $form, FormStateInterface $form_state) {
    $valid_keys = ['[name]', '[type]', '[description]'];

    // This form state contains settings for all of the fields for the
    // current content type, we only validate our own field.
    $field_values = $form_state->getValue('fields');
    foreach ($field_values as $field => $field_settings) {
      if (($field_settings['type'] == 'chado_contact_formatter_default')
          and (array_key_exists('settings_edit_form', $field_settings))) {
        $token_string = $field_settings['settings_edit_form']['settings']['token_string'];
        if ($token_string) {
          $n_tokens = 0;
          $n_invalid = 0;
          // Extract a list of tokens, we need at least one.
          preg_match_all('/\[[^\[\]]*\]/', $token_string, $matches);
          foreach ($matches[0] as $index => $match) {
            if (in_array($match, $valid_keys)) {
              $n_tokens++;
            }
            else {
              $n_invalid++;
            }
          }

          // @todo Note that this is not highlighting the field with the error
          if ($n_invalid) {
            $form_state->setErrorByName($field.'][settings_edit_form][settings][token_string',
                'The token string contains an invalid token, only "[name]", "[type]", and "[description]" may be used.');
          }
          elseif (!$n_tokens) {
            $form_state->setErrorByName($field.'][settings_edit_form][settings][token_string',
                'The token string must contain at least one of "[name]", "[type]", or "[description]".');
          }
        }
      }
    }
  }

}
