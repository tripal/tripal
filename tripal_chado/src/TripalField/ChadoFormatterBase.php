<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalField\TripalFormatterBase;

/**
 * Defines the Chado field formatter base class.
 */
abstract class ChadoFormatterBase extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);
    $plugin_definition = $this->getPluginDefinition();

    // If specified in the annotation, provide a form element for the token string.
    if (array_key_exists('valid_tokens', $plugin_definition)) {
      $valid_tokens = $plugin_definition['valid_tokens'];
      $valid_tokens_str = '"' . implode('", "', $valid_tokens) . '"';

      $form['token_string'] = [
        '#title' => $this->t('Token string for field display'),
        '#description' => $this->t('You may specify elements in this text box to customize how'
                       . ' records are displayed. The available tokens are: ')
                       . $valid_tokens_str,
        '#type' => 'textfield',
        '#default_value' => $this->getSetting('token_string'),
        '#element_validate' => [[$this, 'settingsFormValidateTokenString']],
      ];
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    // If a settings form was specified in the annotation, provide the label for the settings cog.
    $plugin_definition = $this->getPluginDefinition();
    if (array_key_exists('valid_tokens', $plugin_definition)) {
      $summary[] = $this->t('Set display format');
    }
    return $summary;
  }

  /**
   * Form element validation handler for token strings
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) form.
   */
  public function settingsFormValidateTokenString(array $form, FormStateInterface $form_state) {
    $plugin_definition = $this->getPluginDefinition();
    $id = $plugin_definition['id'];
    if (!isset($plugin_definition['valid_tokens'])) {
      return;
    }
    $valid_tokens = $plugin_definition['valid_tokens'];
    $valid_tokens_str = '"' . implode('", "', $valid_tokens) . '"';

    // This form state contains settings for all of the fields for the
    // current content type, we only validate our own field.
    $field_values = $form_state->getValue('fields');
    foreach ($field_values as $field => $field_settings) {
      if (($field_settings['type'] == $id)
          and (array_key_exists('settings_edit_form', $field_settings))) {
        $token_string = $field_settings['settings_edit_form']['settings']['token_string'];
        if ($token_string) {
          $n_tokens = 0;
          $n_invalid = 0;
          // Extract a list of tokens, we need at least one.
          preg_match_all('/\[[^\[\]]*\]/', $token_string, $matches);
          foreach ($matches[0] as $index => $match) {
            if (in_array($match, $valid_tokens)) {
              $n_tokens++;
            }
            else {
              $n_invalid++;
            }
          }

          // Return validation error to the user with offending field highlighted
          if ($n_invalid) {
            $form_state->setErrorByName('fields][' . $field . '][settings_edit_form][settings][token_string',
                'The token string contains an invalid token, valid tokens are: ' . $valid_tokens_str);
          }
          elseif (!$n_tokens) {
            $form_state->setErrorByName('fields][' . $field . '][settings_edit_form][settings][token_string',
                'The token string must contain at least one of: ' . $valid_tokens_str);
          }
        }
      }
    }
  }

}
