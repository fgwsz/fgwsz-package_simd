#!/bin/bash
project_path=$(dirname "$(readlink -f "$0")")
project_name=$(basename "$project_path")
target_path="$project_path/target/linux"
debug_output_path="$target_path/debug"
release_output_path="$target_path/release"
debug_build_type="Debug"
release_build_type="Release"

build(){
    cd "$project_path"
    cmake -B "$debug_output_path" -D CMAKE_BUILD_TYPE="$debug_build_type"
    cmake --build "$debug_output_path" --config "$debug_build_type"
}

build_release(){
    cd "$project_path"
    cmake -B "$release_output_path" -D CMAKE_BUILD_TYPE="$release_build_type"
    cmake --build "$release_output_path" --config "$release_build_type"
}

run(){
    build
    "$debug_output_path/$project_name" "$@"
}

run_release(){
    build_release
    "$release_output_path/$project_name" "$@"
}

clean(){
    if [ -d "$target_path" ]; then
        rm -rf "$target_path"
    fi
}

print_help(){
    cat << EOF
Usage: $0 {build|build_release|run|run_release|clean|help}

Commands:
    build               Build Debug version
    build_release       Build Release version
    run [args]          Build Debug version first, then run the executable (optional arguments)
    run_release [args]  Build Release version first, then run the executable (optional arguments)
    clean               Delete the entire target/linux directory
    help                Show this help message

Examples:
    $0 build
    $0 build_release
    $0 run
    $0 run_release
    $0 clean
    $0 help
EOF
}

main(){
    case "$1" in
        build)
            build
            ;;
        build_release)
            build_release
            ;;
        run)
            shift #Delete first element of $@
            run "$@"
            ;;
        run_release)
            shift
            run_release "$@"
            ;;
        clean)
            clean
            ;;
        help)
            print_help
            ;;
        *)
            print_help
            exit 1
            ;;
    esac
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
