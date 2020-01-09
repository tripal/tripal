<?php
/**
 * @file
 * Contains \Drupal\tripal\Form\Register.
 */
namespace Drupal\tripal\Form;

use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a test form object.
 */
class Register implements FormInterface {
	/*
	public function content() {
		return array(
		  '#type' => 'markup',
		  '#markup' => t('Hello, World!'),
		);
	}
	*/
	
	public function content() {
		//return drupal_get_form('Drupal\tripal\Form\Register');
		$form = \Drupal::formBuilder()->getForm('\Drupal\tripal\Form\Register');
		return $form;		
	}

	/**
	* {@inheritdoc}
	*/
	public function getFormID() {
		return 'tripal_admin_form_register';
	}

	/**
	* {@inheritdoc}
	*/
	public function buildForm(array $form, FormStateInterface $form_state) {
		
		/*
		//TODO: Determine how to deal with $form_state
		$form_data = unserialize(variable_get('tripal_site_registration', NULL));
		$form_state['details']['funding'] = isset($form_state['details']['funding']) ? $form_state['details']['funding'] : 1;
		*/
		
		$form['description'] = [
		'#title' => 'Why Register your Site?',
		'#type' => 'item',
		'#markup' => t('Registering your site is important for continued improvements to the software.  You may opt-in by providing answers to the
			following questions or opt-out by checking the box below. If you opt-in, your site will periodically
			connect to the http://tripal.info website and provide updated registration details. If you opt-out, no
			registration information is communictated. You can opt-out at any time.  If you want previously submitted information
			deleted from the tripal.info database please email !admin.', ['!admin' => Link::fromTextAndUrl('admin@tripal.info', Url::fromUri('mailto:admin@tripal.info'))])
		];
		
		$form['usage_details'] = [
		  '#title' => 'How will this data be used?',
		  '#type' => 'item',
		  '#markup' => t('Tripal is open-source, freely-available, but
			dependent on external funding. When you register your site, it provides important details that can
			be used to justify continued support from funding agencies. The information provided will not be shared publically.
			Information about the Tripal modules installed on your site will be used to help justify continued development to
			funding agencies.  Registration details may be shared with members of Tripal\'s Project Management Committee (PMC) and
			Tripal\'s Steering Committee (TSC) and Tripal extension module usage may be shared with developers
			of the the extension modules to aid in their funding requests.'),
		];
		
		$form['disable_tripal_reporting'] = array(
		'#type' => 'checkbox',
		'#title' => t('Do not register this site (opt-out).'),
		'#default_value' => isset($form_data['values']['disable_tripal_reporting']) ? $form_data['values']['disable_tripal_reporting'] : NULL,
		'#description' => "If you do not want to register your site please check
			this box as it will stop the reminder notifications.  You can return later and register at any time.",
			'#ajax' => array(
				'callback' => '_tripal_form_disable_reg_callback',
				'event' => 'click',
			),
		);
		
		$purpose = array(0 => t('Production'), 1 => t('Development'), 2 => t('Experimental'));
		$form['details']['tripal_reg_site_purpose'] = array(
			'#type' => 'radios',
			'#title' => t('Site Status'),
			'#default_value' => isset($form_data['values']['tripal_reg_site_purpose']) ? $form_data['values']['tripal_reg_site_purpose'] : NULL,
			'#options' => $purpose,
			'#required' => FALSE,
			'#description' => t('Please register your site regardless if it is experimental (to explore tripal), 
			for development of a future site (or updates to an existing site), or a site currently 
			in production. For funding, it is important to know how many sites are active for each category.  If your site changes 
			status, such as from development to production, please remember to return and update the purpose.')
		);
		
		$form['details']['tripal_reg_site_modules'] = array(
			  '#type' => 'checkbox',
			  '#default_value' => isset($form_data['values']['tripal_reg_site_modules']) ? $form_data['values']['tripal_reg_site_modules'] : 1,
			  '#title' => t('Report your installed Tripal Extensions.'),
			  '#description' => t('When checked, any Tripal extension modules that you have installed will be reported with your site\'s registration information.')
		);
		
		$form['details']['tripal_reg_site_description']= array(
			  '#type' => 'textarea',
			  '#title' => t('Description of the site'),
			  '#default_value' => isset($form_data['values']['tripal_reg_site_description']) ? $form_data['values']['tripal_reg_site_description'] : NULL,
			  '#required' => FALSE,
			  '#description' => t('Please provide a brief description of this site.  Consider including
			 details such as its purpose, the primary data types your site provides, and the
			 research community your site serves.')
		);
		
		$form['details']['principal_investigator'] = array(
			'#type' => 'fieldset',
			'#title' => t('Principal Investigator Contact Information'),
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#description' => t('Please provide the name and email of this site\'s principal
			 investigator (PI). If the name and email are provided then the PI agrees to
			 receive periodic communication from either the Tripal Advisory Committee (TAC) or
			 Project Management Committee (PMC) for the purposes of engaging with the larger
			 Tripal user community. The PI will NOT be automatically subscribed to mailing lists.')
		);
		
		$form['details']['principal_investigator']['principal_investigator_name'] = array(
			'#type' => 'textfield',
			'#title' => t('Name'),
			'#default_value' => isset($form_data['values']['principal_investigator_name']) ? $form_data['values']['principal_investigator_name'] : NULL,
			'#required' => FALSE,
		);
		
		$form['details']['principal_investigator']['principal_investigator_email'] = array(
			'#type' => 'textfield',
			'#title' => t('Email'),
			'#default_value' => isset($form_data['values']['principal_investigator_email']) ? $form_data['values']['principal_investigator_email'] : NULL,
			'#required' => FALSE,
		);
		
		$form['details']['tripal_reg_site_admin'] = array(
			'#type' => 'fieldset',
			'#title' => t('Site Manager (if different from the principal investigator)'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
			'#description' => t('Please provide the name and email of this site\'s manager if
			 different from the PI. Sometimes, site managers desire involvement in community
			 activites as well as the PI.  If the name and email are provided then the site manager agrees to
			 receive periodic communication from either the Tripal Advisory Committee (TAC) or
			 Project Management Committee (PMC) for the purposes of engaging with the larger
			 Tripal user community. The site manager will NOT be automatically subscribed to mailing lists.')
		);
		
		$form['details']['tripal_reg_site_admin']['tripal_reg_site_admin_name'] = array(
			'#type' => 'textfield',
			'#title' => t('Name'),
			'#default_value' => isset($form_data['values']['tripal_reg_site_admin_name']) ? $form_data['values']['tripal_reg_site_admin_name'] : NULL,
			'#required' => FALSE,
		);
		
		$form['details']['tripal_reg_site_admin']['tripal_reg_site_admin_email'] = array(
			'#type' => 'textfield',
			'#title' => t('Email'),
			'#default_value' => isset($form_data['values']['tripal_reg_site_admin_email']) ? $form_data['values']['tripal_reg_site_admin_email'] : NULL,
			'#required' => FALSE,
		);
		$form['details']['funding'] = array(
			'#type' => 'container',
			'#tree' => TRUE,
			'#prefix' => '<div id="funding">',
			'#suffix' => '</div>',
		);
		
		$count = count($form_data['values']['funding']);

		/*
		//TODO: Determine how to deal with $form_state
		if ($form_state['details']['funding'] < $count) {
			$form_state['details']['funding'] = $form_state['details']['funding'] + $count;
		}
		else {
			$form_state['details']['funding'] = $form_state['details']['funding'] + 1;
		}

		
		for ($i = 1; $i <= $form_state['details']['funding']; $i++) {
			if ($i === $form_state['details']['funding']) {
			  $form['details']['funding'][$i] = array(
				'#type' => 'fieldset',
				'#title' => t("Funding Source $i"),
				'#tree' => TRUE,
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
				'#description' => t('When requesting funds for additional Tripal development,
			   it is important to report the breadth of of funding sources for Tripal sites.
			   Please consider sharing this information by providing the granting
			   agency, and funding periods.')
			  );
			}
			else {
			  $form['details']['funding'][$i] = array(
				'#type' => 'fieldset',
				'#title' => t("Funding Source $i"),
				'#tree' => TRUE,
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
				'#description' => t('When requesting funds for additional Tripal development,
			   it is important to report the breadth of of funding sources for Tripal sites.
			   Please consider sharing this information by providing the granting
			   agency, and funding periods.')
			  );
			}
			$form['details']['funding'][$i]['tripal_reg_site_agency'] = array(
			  '#type' => 'textfield',
			  '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_agency']) ? $form_data['values']['funding'][$i]['tripal_reg_site_agency'] : NULL,
			  '#title' => t('Funding Agency'),
			);
			$form['details']['funding'][$i]['tripal_reg_site_grant'] = array(
			  '#type' => 'textfield',
			  '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_grant']) ? $form_data['values']['funding'][$i]['tripal_reg_site_grant'] : NULL,
			  '#title' => t('Grant Number'),
			);
			$form['details']['funding'][$i]['tripal_reg_site_amount'] = array(
			  '#type' => 'textfield',
			  '#default_value' => isset($form_data['values']['funding'][$i]['tripal_reg_site_amount']) ? $form_data['values']['funding'][$i]['tripal_reg_site_amount'] : NULL,
			  '#title' => t('Funding Amount'),
			);
			$form['details']['funding'][$i]['funding_period'] = array(
			  '#type' => 'fieldset',
			  '#title' => t('Funding Period'),
			  '#tree' => TRUE,
			);
			$form['details']['funding'][$i]['funding_period']['tripal_reg_site_start'] = array(
			  '#type' => 'date_select',
			  '#title' => t("Start"),
			  '#default_value' => isset($form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_start']) ? $form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_start'] : date('Y', time()),
			  '#date_year_range' => '-20:+20',
			  '#date_format' => 'Y',
			);
			$form['details']['funding'][$i]['funding_period']['tripal_reg_site_end'] = array(
			  '#type' => 'date_select',
			  '#title' => t('End'),
			  '#default_value' => isset($form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_end']) ? $form_data['values']['funding'][$i]['funding_period']['tripal_reg_site_end'] : date('Y', time()),
			  '#date_year_range' => '-20:+20',
			  '#date_format' => 'Y',
			);
		}
		*/
		
		$form['details']['funding']['add_funding'] = array(
			'#type' => 'button',
			'#value' => t('Add additional funding sources'),
			'#href' => '',
			'#ajax' => array(
			  'callback' => 'custom_registration_ajax_add_funding',
			  'wrapper' => 'funding',
			),
		);
		// Provide a submit button.
		if (!empty($form_data)) {
			$form['submit'] = array(
			  '#type' => 'submit',
			  '#value' => 'Update registration information',
			);
		}
		else {
			$form['submit'] = array(
			  '#type' => 'submit',
			  '#value' => 'Register Site',
			);
		}
		return $form;		
	}

	/**
	* {@inheritdoc}
	*/
	public function validateForm(array &$form, FormStateInterface $form_state) {
		drupal_set_message(t('VALIDATION'));
	}

	/**
	* {@inheritdoc}
	*/
	public function submitForm(array &$form, FormStateInterface $form_state) {
		drupal_set_message(t('FORM SUBMIT'));
		/*
		Drupal::config('form_test.object')
		  ->set('bananas', $form_state['values']['bananas'])
		  ->save();
		*/
	}

}

