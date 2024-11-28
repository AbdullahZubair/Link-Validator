<?php

/**
 * @file
 * Batch operations for the Link Validator module.
 * This module validates software download links from app stores.
 */

/**
 * Creates and sets up the batch process for validating links.
 */
function link_validator_batch_create() {
  // Check if the user has permission to run the link validator.
  if (!user_access('run link validator')) {
    drupal_access_denied();
    return;
  }

  try {
    // Get the nodes with links that need validation.
    $nids = link_validator_get_nodes_to_check();

    if (!empty($nids)) {
      $total = count($nids);
      $operations = array();

      // Create a batch operation for each node.
      foreach ($nids as $nid) {
        $operations[] = array(
          'link_validator_process_single_node',
          array($nid, $total),
        );
      }

      // Set up the batch configuration.
      $batch = array(
        'operations' => $operations,
        'finished' => 'link_validator_batch_finished',
        'title' => t('Validating software download links'),
        'init_message' => t('Starting link validation process...'),
        'progress_message' => t('Validated @current out of @total links'),
        'error_message' => t('An error occurred during the link validation process.'),
        'file' => drupal_get_path('module', 'link_validator') . '/link_validator.batch.inc',
      );

      // Initialize the batch process.
      batch_set($batch);
    }
    else {
      drupal_set_message(t('No software nodes with app store links found for validation.'), 'warning');
    }
  }
  catch (Exception $e) {
    // Log any exceptions that occur during batch setup.
    watchdog('link_validator', 'Error creating batch: @message', array('@message' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_set_message(t('An error occurred while preparing the link validation process.'), 'error');
  }
}

/**
 * Processes a single node during the batch operation.
 *
 * @param int $nid
 *   The node ID to process.
 * @param int $total
 *   The total number of nodes being processed.
 * @param array $context
 *   The batch context array.
 */
function link_validator_process_single_node($nid, $total, &$context) {
  // Initialize progress if not set.
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }

  try {
    // Load the node by ID.
    $node = node_load($nid);
    if (!$node) {
      watchdog('link_validator', 'Node @nid not found', array('@nid' => $nid), WATCHDOG_ERROR);
      return;
    }

    // Retrieve download link field items.
    $field_items = field_get_items('node', $node, 'field_software_dl');
    if (!empty($field_items)) {
      foreach ($field_items as $item) {
        if (!empty($item['url']) && link_validator_is_store_url($item['url'])) {
          // Check the link's validity.
          $check_result = link_validator_check_url($item['url']);
          if ($check_result['status'] != 200) {
            // Save the result if the link is invalid.
            link_validator_save_result($nid, $check_result);
          }
        }
      }
    }

    // Update progress and results.
    $context['sandbox']['progress']++;
    $context['message'] = t('Validating "@title"', array('@title' => $node->title));
    $context['results']['processed'][] = $nid;
  }
  catch (Exception $e) {
    // Log any errors encountered during node processing.
    watchdog('link_validator', 'Error processing node @nid: @message', array('@nid' => $nid, '@message' => $e->getMessage()), WATCHDOG_ERROR);
    $context['results']['failed'][] = $nid;
  }
}

/**
 * Callback function triggered when the batch process is complete.
 *
 * @param bool $success
 *   TRUE if the batch completed successfully, FALSE otherwise.
 * @param array $results
 *   An array containing information about processed and failed nodes.
 * @param array $operations
 *   The operations array from the batch.
 */
function link_validator_batch_finished($success, $results, $operations) {
  if ($success) {
    $processed = !empty($results['processed']) ? count($results['processed']) : 0;
    $failed = !empty($results['failed']) ? count($results['failed']) : 0;

    if ($processed > 0) {
      drupal_set_message(t('Successfully validated @count nodes.', array('@count' => $processed)));
    }
    if ($failed > 0) {
      drupal_set_message(t('@failed nodes could not be validated.', array('@failed' => $failed)), 'warning');
    }
  }
  else {
    drupal_set_message(t('Errors occurred while validating links.'), 'error');
  }
}
