import pathlib
import re
import subprocess
import unittest

class TestLab1(unittest.TestCase):

    def _make():
        result = subprocess.run(['make'], capture_output=True, text=True)
        return result

    def _make_clean():
        result = subprocess.run(['make', 'clean'],
                                capture_output=True, text=True)
        return result

    @classmethod
    def setUpClass(cls):
        cls.make = cls._make().returncode == 0

    @classmethod
    def tearDownClass(cls):
        cls._make_clean()

    def test_3_commands(self):
        self.assertTrue(self.make, msg='make failed')
        cl_result = subprocess.run(('ls | cat | wc'),
                                capture_output=True, shell=True)
        pipe_result = subprocess.check_output(('./pipe', 'ls', 'cat', 'wc'))
        self.assertEqual(cl_result.stdout, pipe_result,
            msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result} instead.")
        self.assertTrue(self._make_clean, msg='make clean failed')
    
    def test_no_orphans(self):
        self.assertTrue(self.make, msg='make failed')
        subprocess.call(('strace', '-o', 'trace.log','./pipe','ls','wc','cat','cat'))
        ps = subprocess.Popen(['grep','-o','clone(','trace.log'], stdout=subprocess.PIPE)
        out1 = subprocess.check_output(('wc','-l'), stdin=ps.stdout)
        ps.wait()        
        ps.stdout.close()
        ps = subprocess.Popen(['grep','-o','wait','trace.log'], stdout=subprocess.PIPE)
        out2 = subprocess.check_output(('wc','-l'), stdin=ps.stdout)
        ps.wait()  
        ps.stdout.close()
        out1 = int(out1.decode("utf-8")[0])
        out2 = int(out2.decode("utf-8")[0])
        if out1 == out2 or out1 < out2:
            orphan_check = True
        else:
            orphan_check = False
        self.assertTrue(orphan_check, msg="Found orphan processes")
        subprocess.call(['rm', 'trace.log'])
        self.assertTrue(self._make_clean, msg='make clean failed')
    
    def test_bogus(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run(('./pipe', 'ls', 'bogus'), stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        self.assertTrue(pipe_result.returncode, msg='Bogus argument should cause an error, expect nonzero return code.')
        self.assertNotEqual(pipe_result.stderr, '', msg='Error should be reported to standard error.')
        self.assertTrue(self._make_clean, msg='make clean failed')

    def test_bogus_one_arg(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run(('./pipe', 'bogus'), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.assertTrue(pipe_result.returncode, msg = 'Bogus argument should cause an error, expect nonzero return code.')
        self.assertNotEqual(pipe_result.stderr, '', msg='Error should be reported to standard error.')
        self.assertTrue(self._make_clean, msg='make clean failed')

    def test_bogus_multiple_arg(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run(('./pipe', 'ls', 'bogus', 'wc'), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.assertTrue(pipe_result.returncode, msg = 'Bogus argument should cause an error, expect nonzero return code.')
        self.assertNotEqual(pipe_result.stderr, '', msg='Error should be reported to standard error.')
        self.assertTrue(self._make_clean, msg='make clean failed')

    def test_bogus_multiple_arg_last(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run(('./pipe', 'ls', 'bogus', 'wc'), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.assertTrue(pipe_result.returncode, msg = 'Bogus argument should cause an error, expect nonzero return code.')
        self.assertNotEqual(pipe_result.stderr, '', msg='Error should be reported to standard error.')
        self.assertTrue(self._make_clean, msg='make clean failed')

    def test_file_input(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run('./pipe wc < file.txt', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        cl_result = subprocess.run(('wc < file.txt'), capture_output=True, shell=True)
        self.assertEqual(cl_result.stdout, pipe_result.stdout,
                         msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result.stdout} instead.")
        self.assertTrue(self._make_clean, msg='make clean failed')
    
    def test_file_output(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run(('./pipe ls > file.txt'), capture_output=True, shell=True)
        cl_result = subprocess.run(('ls > file.txt'), capture_output=True, shell=True)
        self.assertEqual(cl_result.stdout, pipe_result.stdout,
                         msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result.stdout} instead.")
        self.assertTrue(self._make_clean, msg='make clean failed')
    def test_ls_format(self):
        self.assertTrue(self.make, msg='make failed')
        pipe_result = subprocess.run(('./pipe ls'), capture_output=True, shell=True)
        cl_result = subprocess.run(('ls'), capture_output=True, shell=True)
        self.assertEqual(cl_result.stdout, pipe_result.stdout,
                         msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result.stdout} instead.")




