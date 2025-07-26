#!/usr/bin/env python3
"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: audit_assets.py
Description: Comprehensive asset library auditing script that counts assets, lists directories, and provides detailed statistics
"""

import os
import sys
from pathlib import Path
from collections import defaultdict
import json
from datetime import datetime

class AssetAuditor:
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.assets_path = self.project_root / "Assets"
        self.stats = {
            'total_files': 0,
            'total_directories': 0,
            'file_types': defaultdict(int),
            'directories': [],
            'largest_files': [],
            'missing_files': [],
            'duplicate_names': [],
            'categories': defaultdict(int),
            'asset_types': {
                'models': 0,
                'textures': 0,
                'audio': 0,
                'video': 0,
                'documents': 0,
                'archives': 0,
                'scripts': 0,
                'other': 0
            },
            'size_breakdown': {
                'tiny': 0,      # < 1 MB
                'small': 0,     # 1-10 MB
                'medium': 0,    # 10-100 MB
                'large': 0,     # 100 MB - 1 GB
                'huge': 0,      # 1-10 GB
                'massive': 0    # > 10 GB
            }
        }
        
    def get_file_size_mb(self, file_path):
        """Get file size in MB - handles ANY size"""
        try:
            return os.path.getsize(file_path) / (1024 * 1024)
        except:
            return 0
    
    def get_file_size_gb(self, file_path):
        """Get file size in GB - for massive files"""
        try:
            return os.path.getsize(file_path) / (1024 * 1024 * 1024)
        except:
            return 0
    
    def format_file_size(self, size_mb):
        """Format file size for display - handles ANY size"""
        if size_mb >= 1024 * 1024:  # > 1 TB
            return f"{size_mb / (1024 * 1024):.2f} TB"
        elif size_mb >= 1024:  # > 1 GB
            return f"{size_mb / 1024:.2f} GB"
        elif size_mb >= 1:  # > 1 MB
            return f"{size_mb:.2f} MB"
        else:  # < 1 MB
            return f"{size_mb * 1024:.0f} KB"
    
    def categorize_file_by_type(self, file_path, ext, size_mb):
        """Categorize file by type and size"""
        # Universal file type detection
        model_extensions = {'.blend', '.obj', '.fbx', '.dae', '.3ds', '.stl', '.ply', '.max', '.c4d', '.ma', '.mb', '.abc', '.usd', '.gltf', '.glb'}
        texture_extensions = {'.png', '.jpg', '.jpeg', '.tga', '.tiff', '.bmp', '.exr', '.hdr', '.psd', '.ai', '.svg', '.webp', '.ktx', '.dds'}
        audio_extensions = {'.mp3', '.wav', '.flac', '.aac', '.ogg', '.wma', '.m4a', '.aiff', '.au', '.mid', '.midi'}
        video_extensions = {'.mp4', '.avi', '.mov', '.wmv', '.flv', '.webm', '.mkv', '.m4v', '.3gp', '.ogv', '.ts', '.mts'}
        document_extensions = {'.pdf', '.doc', '.docx', '.txt', '.rtf', '.md', '.html', '.xml', '.json', '.csv', '.xlsx', '.ppt', '.pptx'}
        archive_extensions = {'.zip', '.rar', '.7z', '.tar', '.gz', '.bz2', '.xz', '.dmg', '.iso'}
        script_extensions = {'.py', '.js', '.php', '.rb', '.java', '.cpp', '.c', '.cs', '.sh', '.bat', '.ps1'}
        
        # Categorize by type
        if ext in model_extensions:
            self.stats['asset_types']['models'] += 1
        elif ext in texture_extensions:
            self.stats['asset_types']['textures'] += 1
        elif ext in audio_extensions:
            self.stats['asset_types']['audio'] += 1
        elif ext in video_extensions:
            self.stats['asset_types']['video'] += 1
        elif ext in document_extensions:
            self.stats['asset_types']['documents'] += 1
        elif ext in archive_extensions:
            self.stats['asset_types']['archives'] += 1
        elif ext in script_extensions:
            self.stats['asset_types']['scripts'] += 1
        else:
            self.stats['asset_types']['other'] += 1
        
        # Categorize by size
        if size_mb < 1:
            self.stats['size_breakdown']['tiny'] += 1
        elif size_mb < 10:
            self.stats['size_breakdown']['small'] += 1
        elif size_mb < 100:
            self.stats['size_breakdown']['medium'] += 1
        elif size_mb < 1024:  # 1 GB
            self.stats['size_breakdown']['large'] += 1
        elif size_mb < 10240:  # 10 GB
            self.stats['size_breakdown']['huge'] += 1
        else:
            self.stats['size_breakdown']['massive'] += 1
    
    def check_for_missing_references(self, file_path, ext):
        """Universal missing file reference detection"""
        # Common reference patterns
        reference_patterns = {
            '.mtl': ['.obj'],  # Material files reference OBJ files
            '.blend': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # Blender files reference textures
            '.max': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # 3DS Max files reference textures
            '.ma': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # Maya files reference textures
            '.mb': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # Maya files reference textures
            '.c4d': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # Cinema 4D files reference textures
            '.fbx': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # FBX files might reference textures
            '.usd': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # USD files might reference textures
            '.gltf': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # glTF files reference textures
            '.glb': ['.png', '.jpg', '.jpeg', '.tga', '.tiff'],  # glTF binary files reference textures
        }
        
        if ext in reference_patterns:
            for ref_ext in reference_patterns[ext]:
                # Check if referenced files exist in same directory
                ref_file = file_path.with_suffix(ref_ext)
                if not ref_file.exists():
                    # Also check in subdirectories
                    found = False
                    for subdir in file_path.parent.iterdir():
                        if subdir.is_dir():
                            potential_ref = subdir / (file_path.stem + ref_ext)
                            if potential_ref.exists():
                                found = True
                                break
                    
                    if not found:
                        self.stats['missing_files'].append(str(ref_file.relative_to(self.project_root)))
    
    def scan_directory(self, directory):
        """Recursively scan directory and collect statistics - UNIVERSAL VERSION"""
        if not directory.exists():
            print(f"❌ Directory not found: {directory}")
            return
            
        print(f"🔍 Scanning: {directory}")
        
        # Universal file type categories
        model_extensions = {'.blend', '.obj', '.fbx', '.dae', '.3ds', '.stl', '.ply', '.max', '.c4d', '.ma', '.mb', '.abc', '.usd', '.gltf', '.glb'}
        texture_extensions = {'.png', '.jpg', '.jpeg', '.tga', '.tiff', '.bmp', '.exr', '.hdr', '.psd', '.ai', '.svg', '.webp', '.ktx', '.dds'}
        audio_extensions = {'.mp3', '.wav', '.flac', '.aac', '.ogg', '.wma', '.m4a', '.aiff', '.au', '.mid', '.midi'}
        video_extensions = {'.mp4', '.avi', '.mov', '.wmv', '.flv', '.webm', '.mkv', '.m4v', '.3gp', '.ogv', '.ts', '.mts'}
        document_extensions = {'.pdf', '.doc', '.docx', '.txt', '.rtf', '.md', '.html', '.xml', '.json', '.csv', '.xlsx', '.ppt', '.pptx'}
        archive_extensions = {'.zip', '.rar', '.7z', '.tar', '.gz', '.bz2', '.xz', '.dmg', '.iso'}
        script_extensions = {'.py', '.js', '.php', '.rb', '.java', '.cpp', '.c', '.cs', '.sh', '.bat', '.ps1'}
        
        for root, dirs, files in os.walk(directory):
            root_path = Path(root)
            
            # Count directories
            self.stats['total_directories'] += len(dirs)
            
            # Add directory to list
            for dir_name in dirs:
                full_dir_path = root_path / dir_name
                self.stats['directories'].append(str(full_dir_path.relative_to(self.project_root)))
                
                # UNIVERSAL categorization - detect ANY category automatically
                dir_lower = dir_name.lower()
                
                # Auto-detect categories from directory names
                if any(keyword in dir_lower for keyword in ['model', 'building', 'character', 'prop', 'mesh', 'geometry']):
                    self.stats['categories']['Models'] += 1
                elif any(keyword in dir_lower for keyword in ['texture', 'material', 'map', 'diffuse', 'normal', 'specular']):
                    self.stats['categories']['Textures'] += 1
                elif any(keyword in dir_lower for keyword in ['audio', 'sound', 'music', 'voice', 'sfx']):
                    self.stats['categories']['Audio'] += 1
                elif any(keyword in dir_lower for keyword in ['video', 'movie', 'animation', 'render', 'footage']):
                    self.stats['categories']['Video'] += 1
                elif any(keyword in dir_lower for keyword in ['scene', 'level', 'environment', 'world']):
                    self.stats['categories']['Scenes'] += 1
                elif any(keyword in dir_lower for keyword in ['script', 'code', 'programming']):
                    self.stats['categories']['Scripts'] += 1
                elif any(keyword in dir_lower for keyword in ['doc', 'manual', 'guide', 'readme']):
                    self.stats['categories']['Documents'] += 1
                elif any(keyword in dir_lower for keyword in ['archive', 'backup', 'compressed']):
                    self.stats['categories']['Archives'] += 1
                else:
                    # Auto-detect from directory name
                    self.stats['categories'][dir_name.title()] += 1
            
            # Process files
            for file in files:
                file_path = root_path / file
                self.stats['total_files'] += 1
                
                # Get file extension
                ext = file_path.suffix.lower()
                self.stats['file_types'][ext] += 1
                
                # Get file size (handles ANY size)
                size_mb = self.get_file_size_mb(file_path)
                
                # Track largest files
                self.stats['largest_files'].append({
                    'path': str(file_path.relative_to(self.project_root)),
                    'size_mb': size_mb
                })
                
                # UNIVERSAL missing file detection
                self.check_for_missing_references(file_path, ext)
                
                # Categorize by file type
                self.categorize_file_by_type(file_path, ext, size_mb)
    
    def find_duplicates(self):
        """Find files with duplicate names"""
        file_names = defaultdict(list)
        
        for root, dirs, files in os.walk(self.assets_path):
            for file in files:
                file_names[file].append(str(Path(root) / file))
        
        for name, paths in file_names.items():
            if len(paths) > 1:
                self.stats['duplicate_names'].append({
                    'name': name,
                    'locations': paths
                })
    
    def generate_report(self):
        """Generate comprehensive audit report"""
        print("\n" + "="*80)
        print("🎨 ASSET LIBRARY AUDIT REPORT")
        print("="*80)
        print(f"📅 Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"📁 Assets Directory: {self.assets_path}")
        print()
        
        # Basic Statistics
        print("📊 BASIC STATISTICS")
        print("-" * 40)
        print(f"📁 Total Directories: {self.stats['total_directories']}")
        print(f"📄 Total Files: {self.stats['total_files']}")
        print()
        
        # File Types
        print("📄 FILE TYPE BREAKDOWN")
        print("-" * 40)
        sorted_types = sorted(self.stats['file_types'].items(), key=lambda x: x[1], reverse=True)
        for ext, count in sorted_types:
            print(f"  {ext:>8}: {count:>4} files")
        print()
        
        # Asset Types
        print("🎨 ASSET TYPE BREAKDOWN")
        print("-" * 40)
        for asset_type, count in self.stats['asset_types'].items():
            if count > 0:
                print(f"  {asset_type:>12}: {count:>4} files")
        print()
        
        # Size Breakdown
        print("📏 SIZE BREAKDOWN")
        print("-" * 40)
        for size_category, count in self.stats['size_breakdown'].items():
            if count > 0:
                print(f"  {size_category:>12}: {count:>4} files")
        print()
        
        # Categories
        print("📂 DIRECTORY CATEGORIES")
        print("-" * 40)
        for category, count in self.stats['categories'].items():
            if count > 0:
                print(f"  {category:>12}: {count:>4} directories")
        print()
        
        # Largest Files
        print("📏 LARGEST FILES (Top 10)")
        print("-" * 40)
        largest_files = sorted(self.stats['largest_files'], key=lambda x: x['size_mb'], reverse=True)[:10]
        for file_info in largest_files:
            formatted_size = self.format_file_size(file_info['size_mb'])
            print(f"  {formatted_size:>10}: {file_info['path']}")
        print()
        
        # Directory Structure
        print("📁 DIRECTORY STRUCTURE")
        print("-" * 40)
        sorted_dirs = sorted(self.stats['directories'])
        for directory in sorted_dirs:
            print(f"  📁 {directory}")
        print()
        
        # Issues
        if self.stats['missing_files']:
            print("⚠️  MISSING FILES")
            print("-" * 40)
            for missing_file in self.stats['missing_files']:
                print(f"  ❌ {missing_file}")
            print()
        
        if self.stats['duplicate_names']:
            print("🔄 DUPLICATE FILE NAMES")
            print("-" * 40)
            for duplicate in self.stats['duplicate_names']:
                print(f"  📄 {duplicate['name']}")
                for location in duplicate['locations']:
                    print(f"      📁 {location}")
            print()
        
        # Summary
        total_size_mb = sum(file_info['size_mb'] for file_info in self.stats['largest_files'])
        print("📋 SUMMARY")
        print("-" * 40)
        print(f"📊 Total Library Size: {self.format_file_size(total_size_mb)}")
        print(f"📁 Directory Count: {self.stats['total_directories']}")
        print(f"📄 File Count: {self.stats['total_files']}")
        print(f"🎨 Asset Types: {sum(self.stats['asset_types'].values())}")
        print(f"⚠️  Issues Found: {len(self.stats['missing_files']) + len(self.stats['duplicate_names'])}")
        print()
        
        # Recommendations
        print("💡 RECOMMENDATIONS")
        print("-" * 40)
        if self.stats['missing_files']:
            print("  🔧 Fix missing referenced files")
        if self.stats['duplicate_names']:
            print("  🔧 Resolve duplicate file names")
        if self.stats['total_files'] > 1000:
            print("  📈 Consider implementing asset versioning")
        if total_size_mb > 1024:  # 1 GB
            print("  💾 Consider implementing asset compression")
        if self.stats['size_breakdown']['huge'] > 0 or self.stats['size_breakdown']['massive'] > 0:
            print("  🚀 Large files detected - consider cloud storage")
        if self.stats['asset_types']['audio'] > 0:
            print("  🎵 Audio files found - consider audio compression")
        if self.stats['asset_types']['video'] > 0:
            print("  🎬 Video files found - consider video compression")
        print("  ✅ Universal library audit complete!")
    
    def save_report(self, filename="asset_audit_report.json"):
        """Save audit report to JSON file"""
        report_data = {
            'timestamp': datetime.now().isoformat(),
            'assets_path': str(self.assets_path),
            'statistics': dict(self.stats),
            'file_types': dict(self.stats['file_types']),
            'categories': dict(self.stats['categories'])
        }
        
        with open(filename, 'w') as f:
            json.dump(report_data, f, indent=2)
        
        print(f"💾 Report saved to: {filename}")
    
    def run_audit(self):
        """Run complete asset audit"""
        print("🎨 Starting Asset Library Audit...")
        print(f"📁 Scanning: {self.assets_path}")
        print()
        
        # Scan assets directory
        self.scan_directory(self.assets_path)
        
        # Find duplicates
        print("🔍 Checking for duplicate files...")
        self.find_duplicates()
        
        # Generate report
        self.generate_report()
        
        # Save report
        self.save_report()
        
        print("✅ Asset audit complete!")

def main():
    """Main function"""
    auditor = AssetAuditor()
    auditor.run_audit()

if __name__ == "__main__":
    main() 