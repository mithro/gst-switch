"""
Integration Tests for the dbus Controller
"""

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os
import pytest
sys.path.insert(0, os.path.abspath(os.path.join(__file__, "../../../")))

from gstswitch.server import Server
from gstswitch.helpers import TestSources, PreviewSinks
from gstswitch.controller import Controller
import time
import datetime

from integrationtests.compare import CompareVideo

import subprocess

# PATH = os.getenv("HOME") + '/gst/stage/bin/'
PATH = '../tools/'


class IntegrationTestbase(object):
    """Base class for integration tests."""

    # pylint: disable=attribute-defined-outside-init
    def setup_method(self, _):
        """Set up called before every test_XXXX method."""
        self.serv = None
        self.sources = None
        self.controller = None

    def setup_server(self):
        """Set up a gst-switch server for testing."""
        assert self.serv is None
        self.serv = Server(path=PATH, video_format="debug")
        self.serv.run()
        self.sources = TestSources(
            video_port=self.serv.video_port, audio_port=self.serv.audio_port)

    def check_port_open(self, port):
        """Check that a socket is open and that you can connect to it."""
        pass

    def setup_controller(self):
        """Create Controller object and call setup_controller."""
        self.controller = Controller()
        self.controller.establish_connection()
        assert self.controller.connection is not None

    def teardown_method(self, _):
        """Tear down called after every test_XXXX method."""
        self.controller = None

        # Kill all the sources
        if self.sources is not None:
            self.sources.terminate_video()
            self.sources.terminate_audio()
        self.sources = None

        if self.serv is not None:
            # Kill the server
            self.serv.terminate(1)

            if self.serv.proc:
                poll = self.serv.proc.poll()
                if poll == -11:
                    print("SEGMENTATION FAULT OCCURRED")
                if poll != 0:
                    print("ERROR CODE - {0}".format(poll))
            log = open('server.log')
            print(log.read())

        self.serv = None


class TestEstablishConnection(IntegrationTestbase):
    """Test setup_controller method"""

    def test_establish(self):
        """Test for setup_controller"""
        self.setup_server()
        self.setup_controller()


class TestGetPorts(IntegrationTestbase):
    """Test get_xxxx_port methods"""

    def test_compose_port(self):
        """Test get_compose_port"""
        self.setup_server()
        self.setup_controller()
        assert self.controller.get_compose_port() == 3001

    def test_encode_port(self):
        """Test get_encode_port"""
        self.setup_server()
        self.setup_controller()
        assert self.controller.get_encode_port() == 3002

    def test_audio_port(self):
        """Test get_encode_port"""
        self.setup_server()
        self.setup_controller()
        assert self.controller.get_encode_port() == 3002

    def test_preview_ports(self):
        """Test get_compose_port"""
        self.setup_server()
        self.setup_controller()

        # Server with no sources should have no compose ports.
        assert self.controller.get_preview_ports() == []

        # Add a test video
        self.sources.new_test_video()
        assert self.controller.get_preview_ports() == [3003]

        # Add a second test video
        self.sources.new_test_video()
        assert self.controller.get_preview_ports() == [3003, 3004]

        # Add a third source, this time audio
        self.sources.new_test_audio()
        assert self.controller.get_preview_ports() == [3003, 3004, 3005]

        # Kill the video sources
        self.sources.terminate_video()
        time.sleep(5)
        assert self.controller.get_preview_ports() == [3005]

        # Add a video source back
        self.sources.new_test_video()
        assert self.controller.get_preview_ports() == [3003, 3005]


class VideoFileSink(object):

    """Sink the video to a file
    """

    def __init__(self, port, filename):
        cmd = "gst-launch-1.0 tcpclientsrc port={0} ! gdpdepay !  jpegenc \
        ! avimux ! filesink location={1}".format(port, filename)
        with open(os.devnull, 'w') as tempf:
            self.proc = subprocess.Popen(
                cmd.split(),
                stdout=tempf,
                stderr=tempf,
                bufsize=-1,
                shell=False)

    def terminate(self):
        """Terminate sinking"""
        self.proc.terminate()


class TestSetCompositeMode(object):

    """Test set_composite_mode method"""
    NUM = 1
    FACTOR = 1

    def set_composite_mode(self, mode, generate_frames=False):
        """Create Controller object and call set_composite_mode method"""
        for _ in range(self.NUM):

            serv = Server(path=PATH, video_format="debug")
            try:
                serv.run()

                preview = PreviewSinks()
                preview.run()

                out_file = 'output-{0}.data'.format(mode)
                video_sink = VideoFileSink(serv.video_port + 1, out_file)

                sources = TestSources(video_port=3000)
                sources.new_test_video(pattern=4)
                sources.new_test_video(pattern=5)

                time.sleep(3)
                # expected_result = [mode != 3] * self.FACTOR
                # print(mode, expected_result)
                controller = Controller()
                res = controller.set_composite_mode(mode)
                print(res)
                time.sleep(3)
                video_sink.terminate()
                preview.terminate()
                sources.terminate_video()
                serv.terminate(1)
                if not generate_frames:
                    controller = Controller()
                    if mode == Controller.COMPOSITE_DUAL_EQUAL:
                        assert res is False
                    else:
                        assert res is True
                    assert self.verify_output(mode, out_file) is True
                # assert expected_result == res

            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())

    def verify_output(self, mode, video):
        """Verify if the output is correct by comparing key frames"""
        test = 'composite_mode_{0}'.format(mode)
        cmpr = CompareVideo(test, video)
        res1, res2 = cmpr.compare()
        print("RESULTS", res1, res2)

        # In the CI environment, upload to imgur the results.
        if os.environ.get('CI', "False") == "true":
            folder = cmpr.test_frame_dir
            cmd = "./imgurbash.sh {0}/*.*".format(folder)
            print(cmd)
            proc = subprocess.Popen(
                cmd,
                bufsize=-1,
                shell=True)
            print(proc.wait())

        # Experimental Value
        if res1 <= 0.04 and res2 <= 0.04:
            return True
        return False

    def test_set_composite_mode_none(self):
        """Test set_composite_mode"""
        self.set_composite_mode(Controller.COMPOSITE_NONE)

    def test_set_composite_mode_pip(self):
        """Test set_composite_mode"""
        self.set_composite_mode(Controller.COMPOSITE_PIP)

    def test_set_composite_mode_preview(self):
        """Test set_composite_mode"""
        self.set_composite_mode(Controller.COMPOSITE_DUAL_PREVIEW)

    def test_set_composite_mode_equal(self):
        """Test set_composite_mode"""
        self.set_composite_mode(Controller.COMPOSITE_DUAL_EQUAL)


class TestNewRecord(object):

    """Test new_record method"""
    NUM = 1
    FACTOR = 1

    def new_record(self):
        """Create a Controller object and call new_record method"""
        res = []
        controller = Controller()
        for _ in range(self.NUM * self.FACTOR):
            res.append(controller.new_record())
        return res

    def test_new_record(self):
        """Test new_record"""
        for _ in range(self.NUM):
            serv = Server(path=PATH, record_file="test-%Y.data",
                          video_format="debug")
            try:
                serv.run()

                sources = TestSources(video_port=3000)
                sources.new_test_video()
                sources.new_test_video()

                curr_time = datetime.datetime.now()
                time_str = curr_time.strftime('%Y')
                test_filename = "test-{0}.data".format(time_str)

                res = self.new_record()
                print(res)
                sources.terminate_video()
                serv.terminate(1)
                assert os.path.exists(test_filename) is True
            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())


class TestAdjustPIP(object):

    """Test adjust_pip method"""
    NUM = 1
    FACTOR = 1

    def adjust_pip(self,
                   xpos,
                   ypos,
                   width,
                   heigth,
                   index,
                   generate_frames=False):
        """Create Controller object and call adjust_pip"""
        for _ in range(self.NUM):
            serv = Server(path=PATH, video_format="debug")
            try:
                serv.run()
                sources = TestSources(video_port=3000)
                preview = PreviewSinks()
                preview.run()
                out_file = "output-{0}.data".format(index)
                video_sink = VideoFileSink(3001, out_file)
                sources.new_test_video(pattern=4)
                sources.new_test_video(pattern=5)
                controller = Controller()
                controller.set_composite_mode(Controller.COMPOSITE_PIP)
                time.sleep(3)
                res = controller.adjust_pip(xpos, ypos, width, heigth)
                time.sleep(3)
                sources.terminate_video()
                preview.terminate()
                video_sink.terminate()
                serv.terminate(1)
                if not generate_frames:
                    assert res is not None
                    assert self.verify_output(index, out_file) is True

            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())

    def verify_output(self, index, video):
        """Verify if the output is correct by comparing key frames"""
        test = 'adjust_pip_{0}'.format(index)
        cmpr = CompareVideo(test, video)
        res1, res2 = cmpr.compare()
        print("RESULTS", res1, res2)
        #   Experimental Value
        if res1 <= 0.04 and res2 <= 0.04:
            return True
        return False

    @pytest.mark.xfail
    def test_adjust_pip(self):
        """Test adjust_pip"""
        dic = [
            [50, 75, 0, 0],
        ]
        for i in range(4, 5):
            self.adjust_pip(
                dic[i - 4][0],
                dic[i - 4][1],
                dic[i - 4][2],
                dic[i - 4][3],
                i)


class TestSwitch(object):

    """Test switch method"""

    NUM = 1

    def switch(self, channel, port, index):
        """Create Controller object and call switch method"""
        for _ in range(self.NUM):
            serv = Server(path=PATH, video_format="debug")
            try:
                serv.run()

                sources = TestSources(3000)
                sources.new_test_video(pattern=4)
                sources.new_test_video(pattern=5)
                preview = PreviewSinks(3001)
                preview.run()
                out_file = "output-{0}.data".format(index)
                video_sink = VideoFileSink(3001, out_file)
                time.sleep(3)
                controller = Controller()
                res = controller.switch(channel, port)
                print(res)
                time.sleep(3)
                video_sink.terminate()
                sources.terminate_video()
                preview.terminate()
                serv.terminate(1)

            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())

    def test_switch(self):
        """Test switch"""
        dic = [
            [Controller.VIDEO_CHANNEL_A, 3004]
        ]
        start = 5
        for i in range(start, 6):
            self.switch(dic[i - start][0], dic[i - start][1], i)


class TestClickVideo(object):

    """Test click_video method"""
    NUM = 1
    FACTOR = 1

    def click_video(self,
                    xpos,
                    ypos,
                    width,
                    heigth,
                    index,
                    generate_frames=False):
        """Create Controller object and call click_video method"""
        for _ in range(self.NUM):
            serv = Server(path=PATH, video_format="debug")
            try:
                serv.run()
                sources = TestSources(video_port=3000)
                preview = PreviewSinks()
                preview.run()
                out_file = "output-{0}.data".format(index)
                video_sink = VideoFileSink(3001, out_file)
                sources.new_test_video(pattern=4)
                sources.new_test_video(pattern=5)
                controller = Controller()
                time.sleep(1)
                res = controller.click_video(xpos, ypos, width, heigth)
                print(res)
                time.sleep(1)
                sources.terminate_video()
                preview.terminate()
                video_sink.terminate()
                serv.terminate(1)
                if not generate_frames:
                    assert res is not None
                    assert self.verify_output(index, out_file) is True

            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())

    def verify_output(self, index, video):
        """Verify if the output is correct by comparing key frames"""
        test = 'click_video_{0}'.format(index)
        cmpr = CompareVideo(test, video)
        res1, res2 = cmpr.compare()
        print("RESULTS", res1, res2)
        #   Experimental Value
        if res1 <= 0.04 and res2 <= 0.04:
            return True
        return False

    def test_click_video(self):
        """Test click_video"""
        dic = [
            [0, 0, 10, 10],
        ]
        start = 6
        for i in range(start, 7):
            self.click_video(
                dic[i - start][0],
                dic[i - start][1],
                dic[i - start][2],
                dic[i - start][3],
                i,
                True)


class TestMarkFace(object):

    """Test mark_face method"""
    NUM = 1

    def mark_face(self, faces, index, generate_frames=False):
        """Create the Controller object and call mark_face method"""
        for _ in range(self.NUM):
            serv = Server(path=PATH, video_format="debug")
            try:
                serv.run()
                sources = TestSources(video_port=3000)
                preview = PreviewSinks()
                preview.run()
                out_file = "output-{0}.data".format(index)
                video_sink = VideoFileSink(3001, out_file)
                sources.new_test_video(pattern=4)
                sources.new_test_video(pattern=5)
                controller = Controller()
                time.sleep(1)
                res = controller.mark_face(faces)
                print(res)
                time.sleep(1)
                sources.terminate_video()
                preview.terminate()
                video_sink.terminate()
                serv.terminate(1)
                if not generate_frames:
                    assert res is not None
                    assert self.verify_output(index, out_file) is True

            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())

    def verify_output(self, index, video):
        """Verify if the output is correct by comparing key frames"""
        test = 'mark_face_{0}'.format(index)
        cmpr = CompareVideo(test, video)
        res1, res2 = cmpr.compare()
        print("RESULTS", res1, res2)
        #   Experimental Value
        if res1 <= 0.04 and res2 <= 0.04:
            return True
        return False

    def test_mark_face(self):
        """Test mark_face"""
        dic = [
            [(1, 1, 1, 1), (10, 10, 1, 1)],
        ]
        start = 7
        for i in range(start, 8):
            self.mark_face(dic[i - start], i, True)


class TestMarkTracking(object):

    """Test mark_tracking method"""
    NUM = 1

    def mark_tracking(self, faces, index, generate_frames=False):
        """Create Controller object and call mark_tracking method"""
        for _ in range(self.NUM):
            serv = Server(path=PATH, video_format="debug")
            try:
                serv.run()
                sources = TestSources(video_port=3000)
                preview = PreviewSinks()
                preview.run()
                out_file = "output-{0}.data".format(index)
                video_sink = VideoFileSink(3001, out_file)
                sources.new_test_video(pattern=4)
                sources.new_test_video(pattern=5)
                controller = Controller()
                time.sleep(1)
                res = controller.mark_tracking(faces)
                print(res)
                time.sleep(1)
                sources.terminate_video()
                preview.terminate()
                video_sink.terminate()
                serv.terminate(1)
                if not generate_frames:
                    assert res is not None
                    assert self.verify_output(index, out_file) is True

            finally:
                if serv.proc:
                    poll = serv.proc.poll()
                    print(self.__class__)
                    if poll == -11:
                        print("SEGMENTATION FAULT OCCURRED")
                    print("ERROR CODE - {0}".format(poll))
                    serv.terminate(1)
                    log = open('server.log')
                    print(log.read())

    def verify_output(self, index, video):
        """Verify if the output is correct by comparing key frames"""
        test = 'mark_tracking_{0}'.format(index)
        cmpr = CompareVideo(test, video)
        res1, res2 = cmpr.compare()
        print("RESULTS", res1, res2)
        #   Experimental Value
        if res1 <= 0.04 and res2 <= 0.04:
            return True
        return False

    def test_mark_tracking(self):
        """Test mark_tracking"""
        dic = [
            [(1, 1, 1, 1), (10, 10, 1, 1)],
        ]
        start = 7
        for i in range(start, 8):
            self.mark_tracking(dic[i - start], i, True)
