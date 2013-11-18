<?php
/**
 * @file
 * Provides the 'find-callback' Drush command.
 *
 * This custom command returns the name of the module that implements a given
 * callback by path (either a full path, or a partial match).
 */

/**
 * The default count to use for the page-timer command.
 */
define('DRUSHUTILS_PAGE_TIMER_COUNT_DEFAULT', 10);


/**
 * Implements COMMANDFILE_drush_command().
 *
 * @return array
 *   Array of Drush command definitions.
 */
function drushutils_drush_command() {
  $items = array();

  $items['find-callback'] = array(
    'description' => "Finds the module(s) that implements a callback by exact or partial path.",
    'arguments' => array(
      'path' => "The callback path to find (e.g. 'views/ajax')",
    ),
    'options' => array(
      'match' => "One of 'full' or 'partial'. Whether to match on an exact path or a partial path. Defaults to 'full'.",
    ),
    'examples' => array(
      'drush find-callback views/ajax' => "Find modules that implement the exact callback 'views/ajax'.",
      'drush find-callback views/ajax --match=partial' => "Find modules that implement the callbacks containing 'views/ajax'.",
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $items['page-timer'] = array(
    'description' => 'A quick and dirty average page execution timer.',
    'options' => array(
      'show-all' => 'If present, this option will tell the command to show the page execution times for each page load.',
      'count' => array(
        'description' => 'The number of page loads to execute for the average page execution time. Defaults to 10.',
        // @todo The next two keys aren't working.
        'example_value' => '10',
        'value' => 'required',
      ),
    ),
  );

  return $items;
}

/**
 * Find Callback command callback.
 */
function drush_drushutils_find_callback() {
  $matches = array();
  $match_type = drush_get_option('match');

  $args = drush_get_arguments();
  $path_to_find = $args[1];

  foreach (module_implements('menu') as $module) {
    $menu_callbacks = module_invoke($module, 'menu');
    if (!empty($menu_callbacks)) {
      foreach ($menu_callbacks as $path => $callback) {
        switch ($match_type) {
          case 'partial':
            if (FALSE !== strpos($path, $path_to_find)) {
              $matches[] = array($module, $path);
            }
            break;

          case 'full':
          default:
            if ($path == $path_to_find) {
              $matches[] = array($module, $path);
              continue 2;
            }
            break;
        }
      }
    }
  }
  _drushutils_output_results($matches, $match_type);
}

/**
 * Find Callback command validation.
 *
 * @return bool
 *   Response from drush_set_error().
 */
function drush_drushutils_find_callback_validate() {
  $args = drush_get_arguments();
  if (1 == count($args)) {
    return drush_set_error('ERROR', dt('The path (or partial path) to search for must be passed as an argument.'));
  }
  return TRUE;
}

/**
 * Outputs the modules and their matched paths.
 *
 * @param array $matches
 *   An array of {module, path} key-pairs.
 *
 * @param string $match_type
 *   The matching type, 'full' or 'partial'.
 */
function _drushutils_output_results($matches, $match_type) {
  if (!isset($match_type)) {
    $match_type = 'full';
  }
  drush_print(('full' != $match_type ? dt('Partial') : dt('Exact')) . ' matches found:');
  drush_print();
  foreach ($matches as $match) {
    drush_print(dt('Module: @module', array('@module' => $match[0])));
    drush_print(dt('Path: @path', array('@path' => $match[1])));
    drush_print();
  }
  drush_print();
}

/**
 * Page Timer command callback.
 */
function drush_drushutils_page_timer() {
  $count = drush_get_option('count');

  if ('boolean' == gettype($count)) {
    drush_set_error('ERROR', dt('The count option requires a value.'));
  }
  else {
    // If the option is NULL, it was not passed at all so we set a default.
    if (NULL === $count) {
      $count = DRUSHUTILS_PAGE_TIMER_COUNT_DEFAULT;
    }

    // We can't check using gettype(), as both 'a' and '1' passed as an option
    // value are typed as strings. Instead, we cast only act on a positive
    // integer.
    $i = (int) $count;
    if ($i > 0) {
      $pad = strlen((string) $i);
      $total = 0.0;
      for ($j = 1; $j <= $i; $j++) {

        // @todo Need to actually DO something here...
        $t = timer_read('page');

        $total += $t;
        if (drush_get_option('show-all')) {
          drush_print(dt('[@j] Page execution time: @time ms', array(
            // Pad out the iteration number according to the total number
            // of iterations.
            '@j' => sprintf("%0{$pad}d", $j),
            '@time' => sprintf('%01.2f', $t),
          )));
        }
      }
      drush_print(dt('[' . str_repeat('-', $pad) . '] Page execution time: @time ms', array(
        '@time' => sprintf('%01.2f', $total / $i),
      )));
    }
    else {
      drush_set_error('ERROR', dt('There was a problem with the option value passed (@value).', array('@value' => $count)));
    }
  }
}
