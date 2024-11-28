<?php

/**
 * @file
 * Page callbacks for the Link Validator module.
 */

// Include batch operations
module_load_include('inc', 'link_validator', 'link_validator.batch');

/**
 * Page callback for report page.
 *
 * Displays a report table listing software download links, their status,
 * and details about redirection and the last checked timestamp.
 */
function link_validator_report_page() {
  $build = array();
  
  // Add CSS and JavaScript
  drupal_add_css(drupal_get_path('module', 'link_validator') . '/css/link_validator.css');
  drupal_add_js(drupal_get_path('module', 'link_validator') . '/js/link_validator.js');
  
  // Add "Check Links Now" button if user has permission.
  if (user_access('run link validator')) {
    $build['check_form'] = drupal_get_form('link_validator_run_form');
  }
  
  // Define table header with sortable columns.
  $header = array(
    array('data' => t('Title'), 'field' => 'n.title'),
    array('data' => t('URL'), 'field' => 'r.url'),
    array('data' => t('Status'), 'field' => 'r.status'),
    array('data' => t('Redirected'), 'field' => 'r.redirected'),
    array('data' => t('Final URL'), 'field' => 'r.final_url'),
    array('data' => t('Last Checked'), 'field' => 'r.checked', 'sort' => 'desc'),
    array('data' => t('Action'), 'field' => null),
  );
  
  // Query to fetch data for the report table.
  $query = db_select('link_validator_results', 'r')
    ->extend('PagerDefault')
    ->extend('TableSort');
    
  $query->join('node', 'n', 'r.nid = n.nid');
  
  $result = $query
    ->fields('r', array('nid', 'url', 'status', 'status_message', 'redirected', 'final_url', 'checked'))
    ->fields('n', array('title'))
    ->orderByHeader($header)
    ->limit(50)
    ->execute();
    
  // Build rows for the report table based on query results.
  $rows = array();
  foreach ($result as $record) {
    $rows[] = array(
      l($record->title, 'node/' . $record->nid),
      l($record->url, $record->url, array('attributes' => array('target' => '_blank'))),
      $record->status . ' - ' . check_plain($record->status_message),
      $record->redirected ? t('Yes') : t('No'),
      $record->redirected ? l($record->final_url, $record->final_url, array('attributes' => array('target' => '_blank'))) : '-',
      format_date($record->checked, 'short'),
      l(t('Mark as fixed'), 'link-validator/mark-fixed/' . $record->nid . '/' . urlencode($record->url), 
        array(
          'attributes' => array(
            'class' => array('use-ajax', 'button', 'mark-fixed-btn'),
            'data-url' => $record->url,
            'role' => 'button',
          ),
        )
      ),
    );
  }
  
  $build['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No invalid links found.'),
    '#attributes' => array('class' => array('link-validator-table')),
  );
  
  $build['pager'] = array('#theme' => 'pager');
  
  return $build;
}

/**
 * Form callback for running the link validator.
 *
 * Provides a form with a submit button to initiate a link validation batch.
 */
function link_validator_run_form($form, &$form_state) {
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Check Links Now'),
  );
  
  return $form;
}

/**
 * Submit handler for the run form.
 *
 * Triggers the batch process to check software download links.
 */
function link_validator_run_form_submit($form, &$form_state) {
  link_validator_batch_create();
}
