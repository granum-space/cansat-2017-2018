from flask_assets import Environment, Bundle


jquery = Bundle('jquery/jquery.min.js')
bootstrap_js = Bundle('bootstrap/js/bootstrap.min.js')
bootstrap_css = Bundle('bootstrap/css/bootstrap.min.css')
main_css = Bundle('scss/main.scss', filters='libsass', depends='scss/*.scss', output='gen/main.css')
chart = Bundle('Chart/Chart.min.js')
openlayers_js = Bundle('OpenLayers/ol.js')
openlayers_css = Bundle('OpenLayers/ol.css')
mcc_charts = Bundle('js/mcc_charts.js')
mcc_map = Bundle('js/mcc_map.js')

assets = Environment()
assets.register('jq_js', jquery)
assets.register('bs_js', bootstrap_js)
assets.register('bs_css', bootstrap_css)
assets.register('main_css', main_css)
assets.register('chart_js', chart)
assets.register('openlayers_js', openlayers_js)
assets.register('openlayers_css', openlayers_css)
assets.register('mcc_charts', mcc_charts)
assets.register('mcc_map', mcc_map)