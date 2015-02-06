"""
Contains the Base class for integration tests.
"""

import sys
import os
import logging

sys.path.insert(0, os.path.abspath(os.path.join(__file__, "../../../")))
from gstswitch.server import Server
from gstswitch.helpers import TestSources  # PreviewSinks
from gstswitch.controller import Controller

# PATH = os.getenv("HOME") + '/gst/stage/bin/'
PATH = '../tools/'


class IntegrationTestbase(object):
    """Base class for integration tests."""

    # Tests are not allowed to have an __init__ method
    # pylint: disable=attribute-defined-outside-init
    def setup_method(self, _):
        """Set up called automatically before every test_XXXX method."""
        self.log = logging.getLogger()
        self.log.setLevel(logging.DEBUG)
        logging.basicConfig(
            format="%(filename)s:%(lineno)d (%(funcName)s): %(message)s")

        self.serv = None
        self.sources = None
        self.controller = None

    def setup_server(self):
        """Set up a gst-switch server for testing."""
        assert self.serv is None

        self.log.info("setting up Server")
        self.serv = Server(path=PATH, video_format="debug")

        self.log.info("running Server")
        self.serv.run()
        assert self.serv.pid

        self.log.info("waiting for Server to open Controller-Port")
        self.serv.wait_for_output('tcp:host=0.0.0.0,port=5000')

        self.log.info("setting up TestSources")
        self.sources = TestSources(
            video_port=self.serv.video_port,
            audio_port=self.serv.audio_port)

    def setup_controller(self):
        """Create Controller object and call setup_controller."""
        self.log.info("setting up Controller")
        self.controller = Controller()

        self.log.info("connecting Controller to Server")
        self.controller.establish_connection()

        assert self.controller.connection is not None

    def teardown_method(self, _):
        """Tear down called automatically after every test_XXXX method."""
        self.controller = None

        # Kill all the sources
        if self.sources is not None:
            self.log.info("terminating Video-TestSource")
            self.sources.terminate_video()

            self.log.info("terminating Audio-TestSource")
            self.sources.terminate_audio()
        self.sources = None

        if self.serv is not None:
            self.log.info("terminating Server")
            self.serv.terminate(1)

            if self.serv.proc:
                poll = self.serv.proc.poll()
                if poll == -11:
                    self.log.error("Server exited with Segmentation Fault")
                if poll != 0:
                    self.log.error("Server exited Error Eode %s", poll)

            log = open('server.log')
            print(log.read())

        self.serv = None