import os
import logging

from flask import Flask

from .assets import assets
from .redis_store import redis_store

from .blueprints.landing import landing
from .blueprints.plots import plots
from .blueprints.gl_test import gl_test
from .blueprints.sound import sound

app = Flask(
    __name__,
    static_folder="static",
    template_folder="templates"
)

app.config.from_object(os.environ["DRAINBOW_MCC_CONFIG"])
log = logging.getLogger('werkzeug')
log.setLevel(app.config["WEB_LOG_LEVEL"])

assets.init_app(app)
redis_store.init_app(app)

app.register_blueprint(landing)
app.register_blueprint(plots)
app.register_blueprint(gl_test)
app.register_blueprint(sound)



