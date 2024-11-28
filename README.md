# Link Validator Module for Drupal7

## Overview:
This module was developed to ensure content validity before migrating a large software library to a new Drupal site. With around 1,500 software nodes, it was crucial to filter out invalid or outdated entries and avoid migrating unnecessary content. The module provides tools to validate URLs in the software download links and logs invalid entries for manual review.

## Features:
1. **Selective Node Querying**: 
   - Fetches only published software nodes.
   - Filters out nodes with download links pointing to platforms like Google Play Store, Apple App Store, or Microsoft Store.

2. **URL Validation**:
   - Uses `drupal_http_request` to send HTTP requests to URLs and analyzes the response codes.
   - Logs details for URLs returning non-200 status codes (e.g., 404, 403).

3. **Custom Database Logging**:
   - Saves invalid URLs with detailed metadata:
     - Software node ID.
     - Original URL.
     - HTTP response code and message.
     - Redirection status and redirected URL (if applicable).

4. **Administrative Interface**:
   - Displays invalid URLs in a user-friendly table.
   - Includes an AJAX-powered "Mark as Fix" button for instant updates without page reloads.

5. **Ease of Manual Verification**:
   - Configures download link attributes to open in new tabs for quick validation.
   - Applies the same setting to redirected URLs for seamless redirection checks.

## Benefits
- Prevents unnecessary migration of invalid content.
- Improves the quality and reliability of the migrated site.
- Saves time for content verification teams with an intuitive review interface.
