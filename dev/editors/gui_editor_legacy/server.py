#!/usr/bin/env python3
"""
MLP Visual Editor Backend Server
Handles compilation and execution of MLP programs
"""

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import os
import tempfile
import shutil

app = Flask(__name__)
CORS(app)

# Paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
MLP_ROOT = os.path.dirname(BASE_DIR)
PREPROCESSOR = os.path.join(MLP_ROOT, 'stage0', 'mlp_preprocessor.py')
COMPILER = os.path.join(MLP_ROOT, 'c_compiler', 'mlp_compiler')
RUNTIME = os.path.join(MLP_ROOT, 'runtime', 'runtime.c')

@app.route('/')
def index():
    return send_from_directory(BASE_DIR, 'index.html')

@app.route('/designer')
def designer():
    return send_from_directory(BASE_DIR, 'designer.html')

@app.route('/<path:path>')
def static_files(path):
    return send_from_directory(BASE_DIR, path)

@app.route('/api/run', methods=['POST'])
def run_program():
    """Compile and run MLP program"""
    try:
        data = request.json
        code = data.get('code', '')
        
        if not code or code.startswith('# Drag blocks'):
            return jsonify({'success': False, 'error': 'No code provided'})
        
        # Create temp directory
        with tempfile.TemporaryDirectory() as tmpdir:
            # Save source file
            source_file = os.path.join(tmpdir, 'program.mlp')
            with open(source_file, 'w', encoding='utf-8') as f:
                f.write(code)
            
            # Step 1: Preprocess
            ir_file = os.path.join(tmpdir, 'program.ir.mlp')
            result = subprocess.run(
                ['python3', PREPROCESSOR, source_file],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            if result.returncode != 0:
                return jsonify({
                    'success': False,
                    'error': f'Preprocessing failed:\n{result.stderr}'
                })
            
            # Check IR file exists
            if not os.path.exists(ir_file):
                return jsonify({
                    'success': False,
                    'error': 'Preprocessor did not generate IR file'
                })
            
            # Step 2: Compile
            asm_file = os.path.join(tmpdir, 'program.asm')
            result = subprocess.run(
                [COMPILER, ir_file, asm_file],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            if result.returncode != 0:
                return jsonify({
                    'success': False,
                    'error': f'Compilation failed:\n{result.stderr}\n{result.stdout}'
                })
            
            # Step 3: Assemble
            obj_file = os.path.join(tmpdir, 'program.o')
            result = subprocess.run(
                ['nasm', '-f', 'elf64', asm_file, '-o', obj_file],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            if result.returncode != 0:
                return jsonify({
                    'success': False,
                    'error': f'Assembly failed:\n{result.stderr}'
                })
            
            # Step 4: Link
            exe_file = os.path.join(tmpdir, 'program')
            result = subprocess.run(
                ['gcc', '-no-pie', obj_file, RUNTIME, '-o', exe_file],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            if result.returncode != 0:
                return jsonify({
                    'success': False,
                    'error': f'Linking failed:\n{result.stderr}'
                })
            
            # Step 5: Execute
            result = subprocess.run(
                [exe_file],
                capture_output=True,
                text=True,
                timeout=10,
                cwd=tmpdir
            )
            
            if result.returncode != 0:
                return jsonify({
                    'success': False,
                    'error': f'Execution failed (exit code {result.returncode}):\n{result.stderr}'
                })
            
            return jsonify({
                'success': True,
                'output': result.stdout
            })
            
    except subprocess.TimeoutExpired:
        return jsonify({'success': False, 'error': 'Execution timeout (>10s)'})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/api/health', methods=['GET'])
def health():
    """Health check endpoint"""
    return jsonify({
        'status': 'ok',
        'preprocessor': os.path.exists(PREPROCESSOR),
        'compiler': os.path.exists(COMPILER),
        'runtime': os.path.exists(RUNTIME)
    })

if __name__ == '__main__':
    print('🚀 MLP Visual Editor Server')
    print(f'   Preprocessor: {PREPROCESSOR}')
    print(f'   Compiler: {COMPILER}')
    print(f'   Runtime: {RUNTIME}')
    print('\n📖 Open: http://localhost:5000\n')
    app.run(host='127.0.0.1', port=5000, debug=False, use_reloader=False)
