(function ($) {
  Drupal.behaviors.linkValidatorMarkFixed = {
    attach: function (context, settings) {
      $('.mark-fixed-btn', context).once('link-validator').click(function(e) {
        e.preventDefault();
        var $button = $(this);
        var $row = $button.closest('tr');
        var url = $button.attr('data-url');
        
        $.ajax({
          url: this.href,
          dataType: 'json',
          beforeSend: function() {
            $button.attr('disabled', 'disabled')
                  .css('opacity', '0.5');
          },
          success: function(response) {
            if (response.success) {
              // Create status message
              var message = $('<div>', {
                'class': 'messages status',
                'role': 'alert',
                'aria-live': 'polite'
              }).text(response.message);
              
              // Fade out the row
              $row.fadeOut(400, function() {
                // Insert message before table
                $('.link-validator-table').before(message);
                
                // Remove the row
                $(this).remove();
                
                // Fade out message after 3 seconds
                setTimeout(function() {
                  message.fadeOut(400, function() {
                    $(this).remove();
                  });
                }, 3000);
              });
            }
          },
          error: function() {
            alert(Drupal.t('An error occurred while marking the URL as fixed.'));
            $button.removeAttr('disabled')
                  .css('opacity', '1');
          }
        });
      });
    }
  };
})(jQuery);
